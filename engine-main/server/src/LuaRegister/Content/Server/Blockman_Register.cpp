#include "Blockman/Entity/EntityPlayerMP.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Blockman/Entity/EntityTrackerEntry.h"
#include "Blockman/World/AnvilManagerServer.h"
#include "Blockman/World/AnvilServer.h"
#include "Blockman/World/BlockChangeRecorderServer.h"
#include "Blockman/Chunk/ChunkServer.h"
#include "Blockman/World/ServerWorld.h"
#include "Blockman/World/ServerWorldProvider.h"
#include "Blockman/World/WorldServerListener.h"
#include "Inventory/InventoryPlayer.h"
#include "LuaRegister/Template/LuaRegister.h"
using namespace BLOCKMAN;

using namespace LUA_REGISTER;


// Register Class EntityPlayerMP
_BEGIN_REGISTER_CLASS(EntityPlayerMP)
_CLASSREGISTER_AddBaseClass(EntityPlayer)
_CLASSREGISTER_AddBaseClass(ICrafting)
_CLASSREGISTER_AddCtor(World *, const String &, const Vector3i &)
_CLASSREGISTER_AddMember(addMoveSpeedPotionEffect, EntityPlayerMP::addMoveSpeedPotionEffect)
_CLASSREGISTER_AddMember(addJumpPotionEffect, EntityPlayerMP::addJumpPotionEffect)
_CLASSREGISTER_AddMember(addNightVisionPotionEffect, EntityPlayerMP::addNightVisionPotionEffect)
_CLASSREGISTER_AddMember(addPoisonPotionEffect, EntityPlayerMP::addPoisonPotionEffect)
_CLASSREGISTER_AddMember(addEffect, EntityPlayerMP::addEffect)
_CLASSREGISTER_AddMember(removeEffect, EntityPlayerMP::removeEffect)
_CLASSREGISTER_AddMember(clearEffects, EntityPlayerMP::clearEffects)
_CLASSREGISTER_AddMember(changeMaxHealth, EntityPlayerMP::changeMaxHealth)
_CLASSREGISTER_AddMember(addItem, EntityPlayerMP::addItem);
_CLASSREGISTER_AddMember(addGunItem, EntityPlayerMP::addGunItem);
_CLASSREGISTER_AddMember(replaceItem, EntityPlayerMP::replaceItem);
_CLASSREGISTER_AddMember(addItemToEnderChest, EntityPlayerMP::addItemToEnderChest)
_CLASSREGISTER_AddMember(addGunItemToEnderChest, EntityPlayerMP::addGunItemToEnderChest)
_CLASSREGISTER_AddMember(equipArmor, EntityPlayerMP::equipArmor);
_CLASSREGISTER_AddMember(addChatMessage, EntityPlayerMP::addChatMessage)
_CLASSREGISTER_AddMember(addExperienceLevel, EntityPlayerMP::addExperienceLevel)
_CLASSREGISTER_AddMember(addSelfToInternalCraftingInventory, EntityPlayerMP::addSelfToInternalCraftingInventory)
_CLASSREGISTER_AddMember(addStat, EntityPlayerMP::addStat)
_CLASSREGISTER_AddMember(attackEntityFrom, EntityPlayerMP::attackEntityFrom)
_CLASSREGISTER_AddMember(canAttackPlayer, EntityPlayerMP::canAttackPlayer)
_CLASSREGISTER_AddMember(canCommandSenderUseCommand, EntityPlayerMP::canCommandSenderUseCommand)
_CLASSREGISTER_AddMember(clonePlayer, EntityPlayerMP::clonePlayer)
_CLASSREGISTER_AddMember(closeContainer, EntityPlayerMP::closeContainer)
_CLASSREGISTER_AddMember(closeScreen, EntityPlayerMP::closeScreen)
_CLASSREGISTER_AddMember(func_110430_a, EntityPlayerMP::func_110430_a)
_CLASSREGISTER_AddMember(getChatVisibility, EntityPlayerMP::getChatVisibility)
_CLASSREGISTER_AddMember(getEyeHeight, EntityPlayerMP::getEyeHeight)
_CLASSREGISTER_AddMember(getPlayerCoordinates, EntityPlayerMP::getPlayerCoordinates)
_CLASSREGISTER_AddMember(getPlayerIP, EntityPlayerMP::getPlayerIP)
_CLASSREGISTER_AddMember(getRaknetID, EntityPlayerMP::getRaknetID)
_CLASSREGISTER_AddMember(jump, EntityPlayerMP::jump)
_CLASSREGISTER_AddMember(specialJump, EntityPlayerMP::specialJump)
_CLASSREGISTER_AddMember(knockBack, EntityPlayerMP::knockBack)
_CLASSREGISTER_AddMember(mountEntity, EntityPlayerMP::mountEntity)
_CLASSREGISTER_AddMember(mountEntityAndWakeUp, EntityPlayerMP::mountEntityAndWakeUp)
_CLASSREGISTER_AddMember(onCriticalHit, EntityPlayerMP::onCriticalHit)
_CLASSREGISTER_AddMember(onDeath, EntityPlayerMP::onDeath)
_CLASSREGISTER_AddMember(onEnchantmentCritical, EntityPlayerMP::onEnchantmentCritical)
_CLASSREGISTER_AddMember(onItemPickup, EntityPlayerMP::onItemPickup)
_CLASSREGISTER_AddMember(onUpdate, EntityPlayerMP::onUpdate)
_CLASSREGISTER_AddMember(readEntityFromNBT, EntityPlayerMP::readEntityFromNBT)
_CLASSREGISTER_AddMember(requestTexturePackLoad, EntityPlayerMP::requestTexturePackLoad)
_CLASSREGISTER_AddMember(sendChatToPlayer, EntityPlayerMP::sendChatToPlayer)
_CLASSREGISTER_AddMember(sendContainerAndContentsToPlayer, EntityPlayerMP::sendContainerAndContentsToPlayer)
_CLASSREGISTER_AddMember(sendContainerToPlayer, EntityPlayerMP::sendContainerToPlayer)
_CLASSREGISTER_AddMember(sendPlayerAbilities, EntityPlayerMP::sendPlayerAbilities)
_CLASSREGISTER_AddMember(sendProgressBarUpdate, EntityPlayerMP::sendProgressBarUpdate)
_CLASSREGISTER_AddMember(sendSlotContents, EntityPlayerMP::sendSlotContents)
_CLASSREGISTER_AddMember(setGameType, EntityPlayerMP::setGameType)
_CLASSREGISTER_AddMember(setItemInUse, EntityPlayerMP::setItemInUse)
_CLASSREGISTER_AddMember(setPositionAndUpdate, EntityPlayerMP::setPositionAndUpdate)
_CLASSREGISTER_AddMember(setRaknetID, EntityPlayerMP::setRaknetID)
_CLASSREGISTER_AddMember(sleepInBedAt, EntityPlayerMP::sleepInBedAt)
_CLASSREGISTER_AddMember(travelToDimension, EntityPlayerMP::travelToDimension)
_CLASSREGISTER_AddMember(triggerEntityUpdate, EntityPlayerMP::triggerEntityUpdate)
_CLASSREGISTER_AddMember(updateFlyingState, EntityPlayerMP::updateFlyingState)
_CLASSREGISTER_AddMember(updateHeldItem, EntityPlayerMP::updateHeldItem)
_CLASSREGISTER_AddMember(updatePotionMetadata, EntityPlayerMP::updatePotionMetadata)
_CLASSREGISTER_AddMember(useItem, EntityPlayerMP::useItem)
_CLASSREGISTER_AddMember(wakeUpPlayer, EntityPlayerMP::wakeUpPlayer)
_CLASSREGISTER_AddMember(writeEntityToNBT, EntityPlayerMP::writeEntityToNBT)
_CLASSREGISTER_AddMember(getInventory, EntityPlayerMP::getInventory)
_CLASSREGISTER_AddMember(setAllowFlying, EntityPlayerMP::setAllowFlying)
_CLASSREGISTER_AddMember(setWatchMode, EntityPlayerMP::setWatchMode)
_CLASSREGISTER_AddMember(addEchantmentItem, EntityPlayerMP::addEchantmentItem)
_CLASSREGISTER_AddMember(addItemPotion, EntityPlayerMP::addItemPotion)
_CLASSREGISTER_AddMember(changeCurrentItem, EntityPlayerMP::changeCurrentItem)
_CLASSREGISTER_AddMember(setCurrency, EntityPlayerMP::setCurrency)
_CLASSREGISTER_AddMember(addCurrency, EntityPlayerMP::addCurrency)
_CLASSREGISTER_AddMember(subCurrency, EntityPlayerMP::subCurrency)
_CLASSREGISTER_AddMember(getCurrency, EntityPlayerMP::getCurrency)
_CLASSREGISTER_AddMember(changeClothes, EntityPlayerMP::changeClothes)
_CLASSREGISTER_AddMember(resetClothes, EntityPlayerMP::resetClothes)
_CLASSREGISTER_AddMember(addBackpackCapacity, EntityPlayerMP::addBackpackCapacity)
_CLASSREGISTER_AddMember(subBackpackCapacity, EntityPlayerMP::subBackpackCapacity)
_CLASSREGISTER_AddMember(resetBackpack, EntityPlayerMP::resetBackpack)
_CLASSREGISTER_AddMember(isParachute, EntityPlayerMP::isParachute)
_CLASSREGISTER_AddMember(setChangePlayerActor, EntityPlayerMP::setChangePlayerActor)
_CLASSREGISTER_AddMember(setCameraLocked, EntityPlayerMP::setCameraLocked)
_CLASSREGISTER_AddMember(changeInvisible, EntityPlayerMP::changeInvisible)
_CLASSREGISTER_AddMember(changeNamePerspective, EntityPlayerMP::changeNamePerspective)
_CLASSREGISTER_AddMember(changeHeart, EntityPlayerMP::changeHeart)
_CLASSREGISTER_AddMember(showBossStripWithIcon, EntityPlayerMP::showBossStripWithIcon)
_CLASSREGISTER_AddMember(changeDefense, EntityPlayerMP::changeDefense)
_CLASSREGISTER_AddMember(changeAttack, EntityPlayerMP::changeAttack)
_CLASSREGISTER_AddMember(setPersonalShopArea, EntityPlayerMP::setPersonalShopArea)
_CLASSREGISTER_AddMember(changeSwitchableProps, EntityPlayerMP::changeSwitchableProps)
_CLASSREGISTER_AddMember(changeUpgradeProps, EntityPlayerMP::changeUpgradeProps)
_CLASSREGISTER_AddMember(changeEnchantmentProps, EntityPlayerMP::changeEnchantmentProps)
_CLASSREGISTER_AddMember(changeSuperProps, EntityPlayerMP::changeSuperProps)
_CLASSREGISTER_AddMember(setSpeedAdditionLevel, EntityPlayerMP::setSpeedAdditionLevel)
_CLASSREGISTER_AddMember(updateExp, EntityPlayerMP::updateExp)
_CLASSREGISTER_AddMember(playSkillEffect, EntityPlayerMP::playSkillEffect)
_CLASSREGISTER_AddMember(sendSkillEffect, EntityPlayerMP::sendSkillEffect)
_CLASSREGISTER_AddMember(setOccupation, EntityPlayerMP::setOccupation)
_CLASSREGISTER_AddMember(showMask, EntityPlayerMP::showMask)
_CLASSREGISTER_AddMember(setUnlockedCommodities, EntityPlayerMP::setUnlockedCommodities)
_CLASSREGISTER_AddMember(setArmItem, EntityPlayerMP::setArmItem)
_CLASSREGISTER_AddMember(setFire, EntityPlayerMP::setFire)
_CLASSREGISTER_AddMember(addGunDamage, EntityPlayerMP::addGunDamage)
_CLASSREGISTER_AddMember(addGunBulletNum, EntityPlayerMP::addGunBulletNum)
_CLASSREGISTER_AddMember(subGunRecoil, EntityPlayerMP::subGunRecoil)
_CLASSREGISTER_AddMember(subGunFireCd, EntityPlayerMP::subGunFireCd)
_CLASSREGISTER_AddMember(changeCustomProps, EntityPlayerMP::changeCustomProps)
_CLASSREGISTER_AddMember(updateCustomProps, EntityPlayerMP::updateCustomProps)
_CLASSREGISTER_AddMember(setOnFrozen, EntityPlayerMP::setOnFrozen)
_CLASSREGISTER_AddMember(getRanch, EntityPlayerMP::getRanch)
_CLASSREGISTER_AddMember(setOnHurt, EntityPlayerMP::setOnHurt)
_CLASSREGISTER_AddMember(setOnColorful, EntityPlayerMP::setOnColorful)
_CLASSREGISTER_AddMember(setDisableMove, EntityPlayerMP::setDisableMove)
_CLASSREGISTER_AddMember(getDiamond, EntityPlayerMP::getDiamond)
_CLASSREGISTER_AddMember(getGold, EntityPlayerMP::getGold)
_CLASSREGISTER_AddMember(setOnFire, EntityPlayerMP::setOnFire)
_CLASSREGISTER_AddMember(addCustomEffect, EntityPlayerMP::addCustomEffect)
_CLASSREGISTER_AddMember(getBirdSimulator, EntityPlayerMP::getBirdSimulator)
_CLASSREGISTER_AddMember(setBagInfo, EntityPlayerMP::setBagInfo)
_CLASSREGISTER_AddMember(setBirdConvert, EntityPlayerMP::setBirdConvert)
_END_REGISTER_CLASS()




// Register Class EntityTracker
_BEGIN_REGISTER_CLASS(EntityTracker)
_CLASSREGISTER_AddBaseClass(Singleton<EntityTracker>)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(World *)
_CLASSREGISTER_AddMember_Override(addEntityToTracker, EntityTracker::addEntityToTracker, void, Entity *)
_CLASSREGISTER_AddMember_Override(addEntityToTracker1, EntityTracker::addEntityToTracker, void, Entity *, int, int, bool)
_CLASSREGISTER_AddMember(getTrackingPlayersOf, EntityTracker::getTrackingPlayersOf)
_CLASSREGISTER_AddMember(removeEntityFromAllTrackingPlayers, EntityTracker::removeEntityFromAllTrackingPlayers)
_CLASSREGISTER_AddMember(removePlayerFromTrackers, EntityTracker::removePlayerFromTrackers)
_CLASSREGISTER_AddMember(sendLeashedEntitiesInChunk, EntityTracker::sendLeashedEntitiesInChunk)
_CLASSREGISTER_AddMember(updateTrackedEntities, EntityTracker::updateTrackedEntities)
_END_REGISTER_CLASS()




// Register Class EntityTrackerEntry
_BEGIN_REGISTER_CLASS(EntityTrackerEntry)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(Entity *, int, int, bool)
_CLASSREGISTER_AddMember(equals, EntityTrackerEntry::equals)
_CLASSREGISTER_AddMember(hashCode, EntityTrackerEntry::hashCode)
_CLASSREGISTER_AddMember(informAllAssociatedPlayersOfItemDestruction, EntityTrackerEntry::informAllAssociatedPlayersOfItemDestruction)
_CLASSREGISTER_AddMember(removeFromWatchingList, EntityTrackerEntry::removeFromWatchingList)
_CLASSREGISTER_AddMember(removePlayerFromTracker, EntityTrackerEntry::removePlayerFromTracker)
_CLASSREGISTER_AddMember(processWatchRemoveRelationTo, EntityTrackerEntry::spawnOrRemoveSelfEntityFor)
_CLASSREGISTER_AddMember(informMovementChangeToTrackingPlayers, EntityTrackerEntry::informMovementChangeToTrackingPlayers)
_CLASSREGISTER_AddMember(watchOrRemoveForEachOtherIfNeed, EntityTrackerEntry::spawnOrRemoveSelfEntityAtClientOf)
_END_REGISTER_CLASS()




// Register Class AnvilManagerServer
_BEGIN_REGISTER_CLASS(AnvilManagerServer)
_CLASSREGISTER_AddBaseClass(AnvilManager)
_END_REGISTER_CLASS()




// Register Class ChunkServerWithMeta
_BEGIN_REGISTER_CLASS(ChunkServerWithMeta)
_END_REGISTER_CLASS()


// Register Class BlockChangeRecorderServer
_BEGIN_REGISTER_CLASS(BlockChangeRecorderServer)
_CLASSREGISTER_AddBaseClass(Singleton<BlockChangeRecorderServer>)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddMember(getChangedPositions, BlockChangeRecorderServer::getChangedPositions)
_CLASSREGISTER_AddMember_Override(record, BlockChangeRecorderServer::record, void, int, int, int)
_CLASSREGISTER_AddMember_Override(record1, BlockChangeRecorderServer::record, void, const BlockPos &)
_END_REGISTER_CLASS()

// Register Class ServerWorld
_BEGIN_REGISTER_CLASS(ServerWorld)
_CLASSREGISTER_AddBaseClass(World)
_CLASSREGISTER_AddMember(fireTNT, ServerWorld::fireTNT)
_CLASSREGISTER_AddMember(addAirDrop, ServerWorld::addAirDrop)
_CLASSREGISTER_AddMember(addCreature, ServerWorld::addCreature)
_CLASSREGISTER_AddMember(addCreatureWithName, ServerWorld::addCreatureWithName)
_CLASSREGISTER_AddMember(addCreatureWithPlayer, ServerWorld::addCreatureWithPlayer)
_CLASSREGISTER_AddMember(setCreatureTeam, ServerWorld::setCreatureTeam)
_CLASSREGISTER_AddMember(killCreature, ServerWorld::killCreature)
_CLASSREGISTER_AddMember(createOrDestroyHouseFromSchematic, ServerWorld::createOrDestroyHouseFromSchematic)
_CLASSREGISTER_AddMember(fillAreaByBlockIdAndMate, ServerWorld::fillAreaByBlockIdAndMate)
_CLASSREGISTER_AddMember(setCropsBlock, ServerWorld::setCropsBlock)
_CLASSREGISTER_AddMember(changeCreatureAttackTarget, ServerWorld::changeCreatureAttackTarget)
_CLASSREGISTER_AddMember(addSkillEffect, ServerWorld::addSkillEffect)
_CLASSREGISTER_AddMember(isInChangeRecord, ServerWorld::isInChangeRecord)
_CLASSREGISTER_AddMember(updateActorNpcContent, ServerWorld::updateActorNpcContent)
_CLASSREGISTER_AddMember(addSimpleEffect, ServerWorld::addSimpleEffect)
_CLASSREGISTER_AddMember(setRailSpeedInfo, ServerWorld::setRailSpeedInfo)
_CLASSREGISTER_AddMember(setRailCarMaxSpeed, ServerWorld::setRailCarMaxSpeed)
_CLASSREGISTER_AddMember(setRailCarCurSpeed, ServerWorld::setRailCarCurSpeed)
_CLASSREGISTER_AddMember(setRailCarAddSpeed, ServerWorld::setRailCarAddSpeed)
_CLASSREGISTER_AddMember(setRailCarRecordRoute, ServerWorld::setRailCarRecordRoute)
_CLASSREGISTER_AddMember(setRailCarActor, ServerWorld::setRailCarActor)
_CLASSREGISTER_AddMember(setRailCarName, ServerWorld::setRailCarName)
_CLASSREGISTER_AddMember(startRailCarRun, ServerWorld::startRailCarRun)
_CLASSREGISTER_AddMember(changeRailCarDirection, ServerWorld::changeRailCarDirection)
_CLASSREGISTER_AddMember(setRailCarFrozen, ServerWorld::setRailCarFrozen)
_CLASSREGISTER_AddMember(updateBuildNpc, ServerWorld::updateBuildNpc)
_CLASSREGISTER_AddMember(playCloseup, ServerWorld::playCloseup)
_CLASSREGISTER_AddMember(setEntitySelected, ServerWorld::setEntitySelected)
_CLASSREGISTER_AddMember(addEntityBulletin, ServerWorld::addEntityBulletin)
_CLASSREGISTER_AddMember(updateEntityBulletin, ServerWorld::updateEntityBulletin)
_CLASSREGISTER_AddMember(addEntityBird, ServerWorld::addEntityBird)
_CLASSREGISTER_AddMember(setBirdDress, ServerWorld::setBirdDress)
_CLASSREGISTER_AddMember(updateBirdDress, ServerWorld::updateBirdDress)
_CLASSREGISTER_AddMember(updateSessionNpc, ServerWorld::updateSessionNpc)
_CLASSREGISTER_AddMember(setFruitsBlock, ServerWorld::setFruitsBlock)
_CLASSREGISTER_AddMember(setEntityHealth, ServerWorld::setEntityHealth)
_CLASSREGISTER_AddMember(setCreatureHome, ServerWorld::setCreatureHome)
_END_REGISTER_CLASS()




// Register Class ChunkServer
_BEGIN_REGISTER_CLASS(ChunkServer)
_CLASSREGISTER_AddBaseClass(Chunk)
_CLASSREGISTER_AddMember(setBlockIDAndMeta, ChunkServer::setBlockIDAndMeta)
_CLASSREGISTER_AddMember(setBlockMeta, ChunkServer::setBlockMeta)
_END_REGISTER_CLASS()




// Register Class BlockEventData
_BEGIN_REGISTER_CLASS(BlockEventData)
_CLASSREGISTER_AddCtor(const BlockPos &, int, int, int)
_END_REGISTER_CLASS()




// Register Class TickEntry
_BEGIN_REGISTER_CLASS(TickEntry)
_CLASSREGISTER_AddCtor()
_CLASSREGISTER_AddCtor(const BlockPos, int)
_END_REGISTER_CLASS()




// Register Class ServerWorldProvider
_BEGIN_REGISTER_CLASS(ServerWorldProvider)
_CLASSREGISTER_AddBaseClass(WorldProvider)
_CLASSREGISTER_AddStaticMember(getProviderForDimension, ServerWorldProvider::getProviderForDimension)
_END_REGISTER_CLASS()




// Register Class ServerWorldProviderEnd
_BEGIN_REGISTER_CLASS(ServerWorldProviderEnd)
_CLASSREGISTER_AddBaseClass(WorldProviderEnd)
_CLASSREGISTER_AddMember(createChunkGenerator, ServerWorldProviderEnd::createChunkGenerator)
_END_REGISTER_CLASS()




// Register Class ServerWorldProviderHell
_BEGIN_REGISTER_CLASS(ServerWorldProviderHell)
_CLASSREGISTER_AddBaseClass(WorldProviderHell)
_CLASSREGISTER_AddMember(createChunkGenerator, ServerWorldProviderHell::createChunkGenerator)
_END_REGISTER_CLASS()




// Register Class ServerWorldProviderSurface
_BEGIN_REGISTER_CLASS(ServerWorldProviderSurface)
_CLASSREGISTER_AddBaseClass(WorldProviderSurface)
_CLASSREGISTER_AddMember(createChunkGenerator, ServerWorldProviderSurface::createChunkGenerator)
_END_REGISTER_CLASS()




// Register Class WorldServerListener
_BEGIN_REGISTER_CLASS(WorldServerListener)
_CLASSREGISTER_AddBaseClass(IWorldEventListener)
_CLASSREGISTER_AddBaseClass(ObjectAlloc)
_CLASSREGISTER_AddCtor(ServerWorld *)
_CLASSREGISTER_AddMember(broadcastSound, WorldServerListener::broadcastSound)
_CLASSREGISTER_AddMember(destroyBlockPartially, WorldServerListener::destroyBlockPartially)
_CLASSREGISTER_AddMember(markBlockForRenderUpdate, WorldServerListener::markBlockForRenderUpdate)
_CLASSREGISTER_AddMember(markBlockForUpdate, WorldServerListener::markBlockForUpdate)
_CLASSREGISTER_AddMember(markBlockRangeForRenderUpdate, WorldServerListener::markBlockRangeForRenderUpdate)
_CLASSREGISTER_AddMember(onEntityCreate, WorldServerListener::onEntityCreate)
_CLASSREGISTER_AddMember(onEntityDestroy, WorldServerListener::onEntityDestroy)
_CLASSREGISTER_AddMember(playAuxSFX, WorldServerListener::playAuxSFX)
_CLASSREGISTER_AddMember(playRecord, WorldServerListener::playRecord)
_CLASSREGISTER_AddMember(playSound, WorldServerListener::playSound)
_CLASSREGISTER_AddMember(playSoundToNearExcept, WorldServerListener::playSoundToNearExcept)
_CLASSREGISTER_AddMember(spawnParticle, WorldServerListener::spawnParticle)
_END_REGISTER_CLASS()




