#ifndef __C2S_PACKETS_H__
#define __C2S_PACKETS_H__
#include <array>
#include "Network/protocol/PacketDefine.h"
#include "Network/NetworkDefine.h"
#include "Math/IntMath.h"
#include "Chat/ChatComponent.h"
#include "World/Manor.h"
#include "Enums/Enums.h"
#include "World/Ranch.h"



//C2SPacketAttack
_BEGIN_PACKET_C2S(C2SPacketType::Attack, DataPacket::emNormal, C2SPacketAttack)
_SERIALIZE_MEMBER(i32, m_targetEntityRuntimeId)
_SERIALIZE_MEMBER(real32, m_targetX)
_SERIALIZE_MEMBER(real32, m_targetY)
_SERIALIZE_MEMBER(real32, m_targetZ)
_END_PACKET

//C2SPacketCheckCSVersion
_BEGIN_PACKET_C2S(C2SPacketType::CheckCSVersion, DataPacket::emNormal, C2SPacketCheckCSVersion)
_SERIALIZE_MEMBER(int, m_version)
_END_PACKET

//C2SPacketLogin
_BEGIN_PACKET_C2S(C2SPacketType::Login, DataPacket::emNormal, C2SPacketLogin)
_SERIALIZE_MEMBER(String, m_gameToken)
_SERIALIZE_MEMBER(String, m_playerName)
_SERIALIZE_MEMBER(ui16, m_netVersion)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(ui64, m_gameTimestamp)
_SERIALIZE_MEMBER(String, m_language)
_END_PACKET

//C2SPacketPlayerMovement
_BEGIN_PACKET_C2S(C2SPacketType::PlayerMovement, DataPacket::emNormal, C2SPacketPlayerMovement)
public:
	C2SPacket()
		: m_x(0.f)
		, m_y(0.f)
		, m_z(0.f)
		, m_yaw(0.f)
		, m_pitch(0.f)
		, m_onGround(false)
		, m_moving(false)
		, m_rotating(false)
	{}

	C2SPacket(float x, float y, float z, float yaw, float pitch, bool onground)
		: m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(yaw)
		, m_pitch(pitch)
		, m_onGround(onground)
		, m_moving(false)
		, m_rotating(false)
	{}

_SERIALIZE_MEMBER(i32, m_playerRuntimeId)
_SERIALIZE_MEMBER(real32, m_x)
_SERIALIZE_MEMBER(real32, m_y)
_SERIALIZE_MEMBER(real32, m_z)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(bool8, m_onGround)
_SERIALIZE_MEMBER(bool8, m_moving)
_SERIALIZE_MEMBER(bool8, m_rotating)
_END_PACKET


_BEGIN_PACKET_C2S(C2SPacketType::VehicleMovement, DataPacket::emNormal, C2SPacketVehicleMovement)
public:
	C2SPacket()
		: m_vehicleId(0)
		, m_x(0.f)
		, m_y(0.f)
		, m_z(0.f)
		, m_yaw(0.f)
		, m_onGround(false)
		, m_moving(false)
		, m_rotating(false)
	{}

	C2SPacket(float x, float y, float z, float yaw, bool onground)
		: m_vehicleId(0)
		, m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(yaw)
		, m_onGround(onground)
		, m_moving(false)
		, m_rotating(false)
	{}

_SERIALIZE_MEMBER(i32, m_playerRuntimeId)
_SERIALIZE_MEMBER(i32, m_vehicleId)
_SERIALIZE_MEMBER(real32, m_x)
_SERIALIZE_MEMBER(real32, m_y)
_SERIALIZE_MEMBER(real32, m_z)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(bool8, m_onGround)
_SERIALIZE_MEMBER(bool8, m_moving)
_SERIALIZE_MEMBER(bool8, m_rotating)
_END_PACKET

//C2SPacketRebirth
_BEGIN_PACKET_C2S(C2SPacketType::Rebirth, DataPacket::emNormal, C2SPacketRebirth)
_SERIALIZE_MEMBER(String, m_playerName)
_SERIALIZE_MEMBER(ui16,  m_netVersion)
_END_PACKET

//C2SPacketAnimate
_BEGIN_PACKET_C2S(C2SPacketType::Animate, DataPacket::emNormal, C2SPacketAnimate)
_SERIALIZE_MEMBER(i32, m_entityRuntimeId)
_SERIALIZE_MEMBER(NETWORK_DEFINE::PacketAnimateType, m_animateType)
_SERIALIZE_MEMBER(i32, m_value)
_END_PACKET

//C2SPacketCraftWithCraftingTable
_BEGIN_PACKET_C2S(C2SPacketType::CraftWithCraftingTable, DataPacket::emNormal, C2SPacketCraftWithCraftingTable)
_SERIALIZE_MEMBER(i32, recipeId)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3i, craftingTablePosition)
_END_PACKET

//C2SPacketCraftWithHand
_BEGIN_PACKET_C2S(C2SPacketType::CraftWithHand, DataPacket::emNormal, C2SPacketCraftWithHand)
_SERIALIZE_MEMBER(i32,recipeId)
_END_PACKET

//C2SPacketChangeItemInHand
_BEGIN_PACKET_C2S(C2SPacketType::ChangeItemInHand, DataPacket::emNormal, C2SPacketChangeItemInHand)
_SERIALIZE_MEMBER(i32, slotIndex)
_END_PACKET

//C2SPacketEquipArmor
_BEGIN_PACKET_C2S(C2SPacketType::EquipArmor, DataPacket::emNormal, C2SPacketEquipArmor)
_SERIALIZE_MEMBER(i32, slotIndex)
_END_PACKET

//C2SPacketUnequipArmor
_BEGIN_PACKET_C2S(C2SPacketType::UnequipArmor, DataPacket::emNormal, C2SPacketUnequipArmor)
_SERIALIZE_MEMBER(i32, slotIndex)
_END_PACKET

//C2SPacketSwapItemPosition
_BEGIN_PACKET_C2S(C2SPacketType::SwapItemPosition, DataPacket::emNormal, C2SPacketSwapItemPosition)
_SERIALIZE_MEMBER(i32, index1)
_SERIALIZE_MEMBER(i32, index2)
_END_PACKET

//C2SPacketDropItemPosition
_BEGIN_PACKET_C2S(C2SPacketType::DropItemPosition, DataPacket::emNormal, C2SPacketDropItemPosition)
_SERIALIZE_MEMBER(i32, inventoryIndex)
_END_PACKET

//C2SPacketPlayerAction
_BEGIN_PACKET_C2S(C2SPacketType::PlayerAction, DataPacket::emNormal, C2SPacketPlayerAction)
_SERIALIZE_MEMBER(NETWORK_DEFINE::PacketPlayerActionType, action)
_END_PACKET

//C2SPacketUseItem
_BEGIN_PACKET_C2S(C2SPacketType::UseItem, DataPacket::emNormal, C2SPacketUseItem)
_SERIALIZE_MEMBER(NETWORK_DEFINE::C2SPacketUseItemUseType, useType)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3i, pos)
_SERIALIZE_MEMBER(i32, param0)
_END_PACKET

//C2SPacketStartBreakBlock
_BEGIN_PACKET_C2S(C2SPacketType::StartBreakBlock, DataPacket::emNormal, C2SPacketStartBreakBlock)
_SERIALIZE_MEMBER(i16, x)
_SERIALIZE_MEMBER(i16, y)
_SERIALIZE_MEMBER(i16, z)
_END_PACKET

//C2SPacketAbortBreakBlock
_BEGIN_PACKET_C2S(C2SPacketType::AbortBreakBlock, DataPacket::emNormal, C2SPacketAbortBreakBlock)
_END_PACKET

//C2SPacketFinishBreakBlock
_BEGIN_PACKET_C2S(C2SPacketType::FinishBreakBlock, DataPacket::emNormal, C2SPacketFinishBreakBlock)
_END_PACKET

//C2SPacketPlaceBlock
_BEGIN_PACKET_C2S(C2SPacketType::PlaceBlock, DataPacket::emNormal, C2SPacketPlaceBlock)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, face)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, hitPos)
_END_PACKET

//C2SPacketActivateBlock
_BEGIN_PACKET_C2S(C2SPacketType::ActivateBlock, DataPacket::emNormal, C2SPacketActivateBlock)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, face)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, hitPos)
_END_PACKET

//C2SPacketExchangeItemStack
_BEGIN_PACKET_C2S(C2SPacketType::ExchangeItemStack, DataPacket::emNormal, C2SPacketExchangeItemStack)
_SERIALIZE_MEMBER(bool8, isPlayerInventoryToChest)
_SERIALIZE_MEMBER(i32, touchTime)
_SERIALIZE_MEMBER(i32, index)
_SERIALIZE_MEMBER(i32, face)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_END_PACKET

//C2SPacketChatString
_BEGIN_PACKET_C2S(C2SPacketType::ChatMessageString, DataPacket::emNormal, C2SPacketChatString)
_SERIALIZE_MEMBER(i32, m_chatType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentString>, m_chatComponent)
_END_PACKET

//C2SPacketLogout
_BEGIN_PACKET_C2S(C2SPacketType::Logout, DataPacket::emNormal, C2SPacketLogout)
_END_PACKET

//C2SPacketUseProp
_BEGIN_PACKET_C2S(C2SPacketType::UseProp, DataPacket::emNormal, C2SPacketUseProp)
_SERIALIZE_MEMBER(String, m_propId)
_END_PACKET

//C2SPacketPing
_BEGIN_PACKET_C2S(C2SPacketType::Ping, DataPacket::emNormal, C2SPacketPing)
_SERIALIZE_MEMBER(ui32, m_sendTime)
_END_PACKET


//C2SPacketLaunchArrow
_BEGIN_PACKET_C2S(C2SPacketType::LaunchArrow, DataPacket::emNormal, C2SPacketLaunchArrow)
_SERIALIZE_MEMBER(real32, cdTime)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, playerPos)
_SERIALIZE_MEMBER(real32, playerYaw)
_SERIALIZE_MEMBER(real32, playerPitch)
_SERIALIZE_MEMBER(ui32, arrowID)
_END_PACKET

//C2SPacketLaunchBullet
_BEGIN_PACKET_C2S(C2SPacketType::LaunchBullet, DataPacket::emNormal, C2SPacketLaunchBullet)
_SERIALIZE_MEMBER(ui32, bulletID)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, playerPos)
_SERIALIZE_MEMBER(real32, playerYaw)
_SERIALIZE_MEMBER(real32, playerPitch)
_END_PACKET

//C2SPacketSpawnVehicle
_BEGIN_PACKET_C2S(C2SPacketType::SpawnVehicle, DataPacket::emNormal, C2SPacketSpawnVehicle)
_SERIALIZE_MEMBER(ui32, carID)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, pos)
_SERIALIZE_MEMBER(real32, yaw)
_END_PACKET


//C2SPacketGunFireResult
_BEGIN_PACKET_C2S(C2SPacketType::GunFireResult, DataPacket::emNormal, C2SPacketGunFireResult)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, pos)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, dir)
_SERIALIZE_MEMBER(ui32, tracyType)						// 0: none  1: block  2: entity(Player)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3i, blockPos)	// if tracyType is 1, the blockPos is the collision Block!
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, hitpos)	// if tracyType is 1 or 2, the hitpos is the collision position!
_SERIALIZE_MEMBER(ui32, hitEntityID)					// if tracyType is 2, the hitEntityID is serverID
_SERIALIZE_MEMBER(bool, isNeedConsumeBullet)
_SERIALIZE_MEMBER(bool, isHeadshot)
_END_PACKET


//C2SPacketSplashPotion
_BEGIN_PACKET_C2S(C2SPacketType::SplashPotion, DataPacket::emNormal, C2SPacketSplashPotion)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, playerPos)
_SERIALIZE_MEMBER(real32, playerYaw)
_SERIALIZE_MEMBER(real32, playerPitch)
_END_PACKET

//C2SPacketCloseContainer
_BEGIN_PACKET_C2S(C2SPacketType::CloseContainer, DataPacket::emNormal, C2SPacketCloseContainer)
_SERIALIZE_MEMBER(i16, containerPosX)
_SERIALIZE_MEMBER(i16, containerPosY)
_SERIALIZE_MEMBER(i16, containerPosZ)
_END_PACKET


//C2SPacketUseThrowableItem
_BEGIN_PACKET_C2S(C2SPacketType::UseThrowableItem, DataPacket::emNormal, C2SPacketUseThrowableItem)
_END_PACKET

//C2SPacketSetFlying
_BEGIN_PACKET_C2S(C2SPacketType::SetFlying, DataPacket::emNormal, C2SPacketSetFlying)
_SERIALIZE_MEMBER(bool8, isFlying)
_END_PACKET

//C2SPacketBuyCommodity
_BEGIN_PACKET_C2S(C2SPacketType::BuyCommodity, DataPacket::emNormal, C2SPacketBuyCommodity)
_SERIALIZE_MEMBER(i32, merchantId)
_SERIALIZE_MEMBER(i32, groupIndex)
_SERIALIZE_MEMBER(i32, commodityIndex)
_SERIALIZE_MEMBER(i32, commodityId)
_END_PACKET

//C2SPacketTakeVehicle
_BEGIN_PACKET_C2S(C2SPacketType::TakeVehicle, DataPacket::emNormal, C2SPacketTakeVehicle)
_SERIALIZE_MEMBER(bool8, onoff)
_SERIALIZE_MEMBER(i32, vehicleId)
_END_PACKET

//C2SGunShootTarget
_BEGIN_PACKET_C2S(C2SPacketType::ReloadBullet, DataPacket::emNormal, C2SReloadBullet)
_END_PACKET

//C2SPacketBuyGoods
_BEGIN_PACKET_C2S(C2SPacketType::BuyGoods, DataPacket::emNormal, C2SPacketBuyGoods)
_SERIALIZE_MEMBER(i32, groupIndex)
_SERIALIZE_MEMBER(i32, goodsIndex)
_SERIALIZE_MEMBER(i32, goodsId)
_END_PACKET

//C2SPacketBuyGoods
_BEGIN_PACKET_C2S(C2SPacketType::ResumeOrder, DataPacket::emNormal, C2SPacketResumeOrder)
_SERIALIZE_MEMBER(String, orderId)
_END_PACKET

_BEGIN_PACKET_C2S(C2SPacketType::SelectRole, DataPacket::emNormal, C2SPacketSelectRole)
_SERIALIZE_MEMBER(i32, role)
_END_PACKET

//C2SPacketBuyVehicle
_BEGIN_PACKET_C2S(C2SPacketType::BuyVehicle, DataPacket::emNormal, C2SPacketBuyVehicle)
_SERIALIZE_MEMBER(i32, vehicleId)
_END_PACKET

//C2SPacketTakeVehicle
_BEGIN_PACKET_C2S(C2SPacketType::LockVehicle, DataPacket::emNormal, C2SPacketLockVehicle)
_SERIALIZE_MEMBER(bool8, onoff)
_SERIALIZE_MEMBER(i32, vehicleId)
_END_PACKET

//C2SPacketClientReady
_BEGIN_PACKET_C2S(C2SPacketType::ClientReady, DataPacket::emNormal, C2SPacketClientReady)
_SERIALIZE_MEMBER(i32, m_status)
_END_PACKET

_BEGIN_PACKET_C2S(C2SPacketType::VehicleAction, DataPacket::emNormal, C2SPacketVehicleActionState)
_SERIALIZE_MEMBER(i32, vehicleId)
_SERIALIZE_MEMBER(i32, state)
_END_PACKET

_BEGIN_PACKET_C2S(C2SPacketType::BuyRespawn, DataPacket::emNormal, C2SPacketBuyRespawn)
_SERIALIZE_MEMBER(bool8, isRespawn)
_SERIALIZE_MEMBER(i32, uniqueId)
_SERIALIZE_MEMBER(i32, index)
_END_PACKET

_BEGIN_PACKET_C2S(C2SPacketType::GoNpcMerchant, DataPacket::emNormal, C2SPacketGoNpcMerchant)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(float, x)
_SERIALIZE_MEMBER(float, y)
_SERIALIZE_MEMBER(float, z)
_END_PACKET

_BEGIN_PACKET_C2S(C2SPacketType::ClickTeleport, DataPacket::emNormal, C2SPacketClickTeleport)
_END_PACKET

//C2SPacketChangeClothes
_BEGIN_PACKET_C2S(C2SPacketType::ChangeClothes, DataPacket::emNormal, C2SPacketChangeClothes)
_SERIALIZE_MEMBER(i32, actorId)
_SERIALIZE_MEMBER(String, partName)
_END_PACKET

//C2SPacketRestClothes
_BEGIN_PACKET_C2S(C2SPacketType::RestClothes, DataPacket::emNormal, C2SPacketRestClothes)
_END_PACKET

//C2SPacketLeaveAircraft
_BEGIN_PACKET_C2S(C2SPacketType::LeaveAircraft, DataPacket::emNormal, C2SPacketLeaveAircraft)
_END_PACKET

//C2SPacketFriendOperation
_BEGIN_PACKET_C2S(C2SPacketType::FriendOperation, DataPacket::emNormal, C2SPacketFriendOperation)
_SERIALIZE_MEMBER(i32, operationType)
_SERIALIZE_MEMBER(ui64, userId)
_END_PACKET

//C2SPacketSetParachute
_BEGIN_PACKET_C2S(C2SPacketType::SetParachute, DataPacket::emNormal, C2SPacketSetParachute)
_SERIALIZE_MEMBER(bool8, isParachute)
_END_PACKET

//C2SPacketSkillType
_BEGIN_PACKET_C2S(C2SPacketType::SkillType, DataPacket::emNormal, C2SPacketSkillType)
_SERIALIZE_MEMBER(int, skillType)
_SERIALIZE_MEMBER(bool8, skillStatus)
_END_PACKET

//C2SPacketChangePlayerActor
_BEGIN_PACKET_C2S(C2SPacketType::ChangePlayerActor, DataPacket::emNormal, C2SPacketChangePlayerActor)
_END_PACKET

//C2SPacketClickChangeActor
_BEGIN_PACKET_C2S(C2SPacketType::ClickChangeActor, DataPacket::emNormal, C2SPacketClickChangeActor)
_END_PACKET

//C2SPacketCameraLockMovement
_BEGIN_PACKET_C2S(C2SPacketType::CameraLockMovement, DataPacket::emNormal, C2SPacketCameraLockMovement)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, nextPosition)
_END_PACKET

//C2SPacketCameraLock
_BEGIN_PACKET_C2S(C2SPacketType::CameraLock, DataPacket::emNormal, C2SPacketCameraLock)
_SERIALIZE_MEMBER(bool8, isLocked)
_END_PACKET

//C2SPacketPickUpItemPay
_BEGIN_PACKET_C2S(C2SPacketType::PickUpItemPay, DataPacket::emNormal, C2SPacketPickUpItemPay)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, itemEntityId)
_END_PACKET

//C2SPacketAttackActorNpc
_BEGIN_PACKET_C2S(C2SPacketType::AttackActorNpc, DataPacket::emNormal, C2SPacketAttackActorNpc)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET


//C2SPacketGetManor
_BEGIN_PACKET_C2S(C2SPacketType::GetManor, DataPacket::emNormal, C2SPacketGetManor)
_END_PACKET

//C2SPacketSellManor
_BEGIN_PACKET_C2S(C2SPacketType::SellManor, DataPacket::emNormal, C2SPacketSellManor)
_END_PACKET

//C2SPacketUpgradeManor
_BEGIN_PACKET_C2S(C2SPacketType::UpgradeManor, DataPacket::emNormal, C2SPacketUpgradeManor)
_END_PACKET

//C2SPacketBuyHouse
_BEGIN_PACKET_C2S(C2SPacketType::BuyHouse, DataPacket::emNormal, C2SPacketBuyHouse)
_SERIALIZE_MEMBER(BLOCKMAN::ManorHouse, m_house)
_END_PACKET

//C2SPacketBuyFurniture
_BEGIN_PACKET_C2S(C2SPacketType::BuyFurniture, DataPacket::emNormal, C2SPacketBuyFurniture)
_SERIALIZE_MEMBER(BLOCKMAN::ManorFurniture, m_furniture)
_END_PACKET

//C2SPacketDeleteManorMessage
_BEGIN_PACKET_C2S(C2SPacketType::DeleteManorMessage, DataPacket::emNormal, C2SPacketDeleteManorMessage)
_SERIALIZE_MEMBER(BLOCKMAN::ManorMessage, m_message)
_END_PACKET

//C2SPacketBuildHouse
_BEGIN_PACKET_C2S(C2SPacketType::BuildHouse, DataPacket::emNormal, C2SPacketBuildHouse)
_SERIALIZE_MEMBER(BLOCKMAN::ManorHouse, m_house)
_END_PACKET


//C2SPacketPutFurniture
_BEGIN_PACKET_C2S(C2SPacketType::PutFurniture, DataPacket::emNormal, C2SPacketPutFurniture)
_SERIALIZE_MEMBER(BLOCKMAN::ManorFurniture, m_furniture)
_SERIALIZE_MEMBER(BLOCKMAN::PutFurnitureStatus, m_status)
_END_PACKET

//C2SPacketRecycleFurniture
_BEGIN_PACKET_C2S(C2SPacketType::RecycleFurniture, DataPacket::emNormal, C2SPacketRecycleFurniture)
_SERIALIZE_MEMBER(BLOCKMAN::ManorFurniture, m_furniture)
_END_PACKET

//C2SPacketManorTeleport
_BEGIN_PACKET_C2S(C2SPacketType::ManorTeleport, DataPacket::emNormal, C2SPacketManorTeleport)
_END_PACKET

//C2SPacketCallOnManor
_BEGIN_PACKET_C2S(C2SPacketType::CallOnManor, DataPacket::emNormal, C2SPacketCallOnManor)
_SERIALIZE_MEMBER(ui64, m_targetUserId)
_END_PACKET

//C2SPacketGiveRose
_BEGIN_PACKET_C2S(C2SPacketType::GiveRose, DataPacket::emNormal, C2SPacketGiveRose)
_SERIALIZE_MEMBER(ui64, m_targetUserId)
_END_PACKET

//C2SPacketRequestUpdateRealTimeRank
_BEGIN_PACKET_C2S(C2SPacketType::RequestUpdateRealTimeRank, DataPacket::emNormal, C2SPacketRequestUpdateRealTimeRank)
_END_PACKET

//C2SPacketBuySwitchableProp
_BEGIN_PACKET_C2S(C2SPacketType::BuySwitchableProp, DataPacket::emNormal, C2SPacketBuySwitchableProp)
_SERIALIZE_MEMBER(String, uniqueId)
_END_PACKET

//C2SPacketBuyUpgradeProp
_BEGIN_PACKET_C2S(C2SPacketType::BuyUpgradeProp, DataPacket::emNormal, C2SPacketBuyUpgradeProp)
_SERIALIZE_MEMBER(String, uniqueId)
_END_PACKET

//C2SPacketAttackCreatureEntity
_BEGIN_PACKET_C2S(C2SPacketType::AttackCreatureEntity, DataPacket::emNormal, C2SPacketAttackCreatureEntity)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//C2SPacketUpgradeResource
_BEGIN_PACKET_C2S(C2SPacketType::UpgradeResource, DataPacket::emNormal, C2SPacketUpgradeResource)
_SERIALIZE_MEMBER(i32, resourceId)
_END_PACKET


//C2SPacketWatchRespawn
_BEGIN_PACKET_C2S(C2SPacketType::WatchRespawn, DataPacket::emNormal, C2SPacketWatchRespawn)
_END_PACKET

//C2SPacketMovementInput
_BEGIN_PACKET_C2S(C2SPacketType::MovementInput, DataPacket::emNormal, C2SPacketMovementInput)
_SERIALIZE_MEMBER(float, forward)
_SERIALIZE_MEMBER(float, strafe)
_END_PACKET

//C2SPacketSendJumpChanged
_BEGIN_PACKET_C2S(C2SPacketType::SendJumpChanged, DataPacket::emNormal, C2SPacketSendJumpChanged)
_SERIALIZE_MEMBER(bool, isJump)
_END_PACKET

//C2SPacketSendPlayerRotation
_BEGIN_PACKET_C2S(C2SPacketType::SendPlayerRotation, DataPacket::emNormal, C2SPacketSendPlayerRotation)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(float, pitch)
_END_PACKET

//C2SPacketDownJumpChanged
_BEGIN_PACKET_C2S(C2SPacketType::SendDownChanged, DataPacket::emNormal, C2SPacketSendDownChanged)
_SERIALIZE_MEMBER(bool, isDown)
_END_PACKET

//C2SPacketNeedUpdateMerchantCommodities
_BEGIN_PACKET_C2S(C2SPacketType::NeedUpdateMerchantCommodities, DataPacket::emNormal, C2SPacketNeedUpdateMerchantCommodities)
_SERIALIZE_MEMBER(i32, merchantId)
_END_PACKET

//C2SPacketSyncCustomTipResult
_BEGIN_PACKET_C2S(C2SPacketType::SyncCustomTipResult, DataPacket::emNormal, C2SPacketSyncCustomTipResult)
_SERIALIZE_MEMBER(String, extra)
_SERIALIZE_MEMBER(bool, bResult)
_END_PACKET

//C2SPacketRelaseItemSkill
_BEGIN_PACKET_C2S(C2SPacketType::RelaseItemSkill, DataPacket::emNormal, C2SPacketRelaseItemSkill)
_SERIALIZE_MEMBER(int, skillItemId)
_END_PACKET

//C2SPacketBuildWarOpenShop
_BEGIN_PACKET_C2S(C2SPacketType::BuildWarOpenShop, DataPacket::emNormal, C2SPacketBuildWarOpenShop)
_END_PACKET

//C2SPacketBuildWarGrade
_BEGIN_PACKET_C2S(C2SPacketType::BuildWarGrade, DataPacket::emNormal, C2SPacketBuildWarGrade)
_SERIALIZE_MEMBER(ui8, grade)
_END_PACKET

//C2SPacketBuildWarOpenGuessResult
_BEGIN_PACKET_C2S(C2SPacketType::BuildWarOpenGuessResult, DataPacket::emNormal, C2SPacketBuildWarOpenGuessResult)
_END_PACKET

//C2SPacketBuildWarGuessVisit
_BEGIN_PACKET_C2S(C2SPacketType::BuildWarGuessVisit, DataPacket::emNormal, C2SPacketBuildWarGuessVisit)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_END_PACKET

//C2SPacketBuildWarGuessSuc
_BEGIN_PACKET_C2S(C2SPacketType::BuildWarGuessSuc, DataPacket::emNormal, C2SPacketBuildWarGuessSuc)
_SERIALIZE_MEMBER(i32, guessRoomId)
_END_PACKET

//C2SPacketShowBuildWarResult
_BEGIN_PACKET_C2S(C2SPacketType::BuildWarShowResult, DataPacket::emNormal, C2SPacketShowBuildWarResult)
_END_PACKET

//C2SPacketGetCommodity
_BEGIN_PACKET_C2S(C2SPacketType::GetCommodity, DataPacket::emNormal, C2SPacketGetCommodity)
_SERIALIZE_MEMBER(i32, merchantId)
_SERIALIZE_MEMBER(i32, groupIndex)
_SERIALIZE_MEMBER(i32, commodityIndex)
_SERIALIZE_MEMBER(i32, commodityId)
_END_PACKET

//C2SPacketUnlockCommodity
_BEGIN_PACKET_C2S(C2SPacketType::UnlockCommodity, DataPacket::emNormal, C2SPacketUnlockCommodity)
_SERIALIZE_MEMBER(i32, merchantId)
_SERIALIZE_MEMBER(i32, groupIndex)
_SERIALIZE_MEMBER(i32, commodityIndex)
_SERIALIZE_MEMBER(i32, commodityId)
_END_PACKET

//C2SPacketPlayerInteractionActionMesage
_BEGIN_PACKET_C2S(C2SPacketType::PlayerInteractionActionMesage, DataPacket::emNormal, C2SPacketPlayerInteractionActionMesage)
_SERIALIZE_MEMBER(ui64, m_targetUserId)
_SERIALIZE_MEMBER(String, m_langKey)
_SERIALIZE_MEMBER(vector<String>::type, args)
_END_PACKET

//C2SPacketAttackEntity
_BEGIN_PACKET_C2S(C2SPacketType::AttackEntity, DataPacket::emNormal, C2SPacketAttackEntity)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//C2SPacketConsumeTipResult
_BEGIN_PACKET_C2S(C2SPacketType::ConsumeTipResult, DataPacket::emNormal, C2SPacketConsumeTipResult)
_SERIALIZE_MEMBER(String, extra)
_SERIALIZE_MEMBER(bool, bResult)
_END_PACKET

//C2SPacketBuyEnchantmentProp
_BEGIN_PACKET_C2S(C2SPacketType::BuyEnchantmentProp, DataPacket::emNormal, C2SPacketBuyEnchantmentProp)
_SERIALIZE_MEMBER(String, uniqueId)
_END_PACKET

//C2SPacketBuySuperProp
_BEGIN_PACKET_C2S(C2SPacketType::BuySuperProp, DataPacket::emNormal, C2SPacketBuySuperProp)
_SERIALIZE_MEMBER(String, uniqueId)
_END_PACKET

//C2SPacketBuyCustomProp
_BEGIN_PACKET_C2S(C2SPacketType::BuyCustomProp, DataPacket::emNormal, C2SPacketBuyCustomProp)
_SERIALIZE_MEMBER(String, uniqueId)
_END_PACKET

//C2SPacketClickBackHall
_BEGIN_PACKET_C2S(C2SPacketType::ClickBackHall, DataPacket::emNormal, C2SPacketClickBackHall)
_END_PACKET

//C2SPacketLongHitEntity
_BEGIN_PACKET_C2S(C2SPacketType::LongHitEntity, DataPacket::emNormal, C2SPacketLongHitEntity)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//C2SPacketUseFishingRod
_BEGIN_PACKET_C2S(C2SPacketType::UseFishingRod, DataPacket::emNormal, C2SPacketUseFishingRod)
_END_PACKET

//C2SPacketUseCannon
_BEGIN_PACKET_C2S(C2SPacketType::UseCannon, DataPacket::emNormal, C2SPacketUseCannon)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//C2SPacketUpdateUserMoney
_BEGIN_PACKET_C2S(C2SPacketType::UpdateUserMoney, DataPacket::emNormal, C2SPacketUpdateUserMoney)
_END_PACKET

//C2SPacketBuyBullet
_BEGIN_PACKET_C2S(C2SPacketType::BuyBullet, DataPacket::emNormal, C2SPacketBuyBullet)
_END_PACKET

//C2SPacketBuyRanchItem
_BEGIN_PACKET_C2S(C2SPacketType::RanchBuyItem, DataPacket::emNormal, C2SPacketRanchBuyItem)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, num)
_SERIALIZE_MEMBER(i32, totalPrice)
_SERIALIZE_MEMBER(i32, currencyType)
_END_PACKET

//C2SPacketRanchBuyBuildItem
_BEGIN_PACKET_C2S(C2SPacketType::RanchBuyBuildItem, DataPacket::emNormal, C2SPacketRanchBuyBuildItem)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, num)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, currencyType)
_END_PACKET

//C2SPacketRanchOperation
_BEGIN_PACKET_C2S(C2SPacketType::RanchOperation, DataPacket::emNormal, C2SPacketRanchOperation)
_SERIALIZE_MEMBER(BLOCKMAN::RanchOperationType, m_type)
_END_PACKET

//C2SPacketRanchReceive
_BEGIN_PACKET_C2S(C2SPacketType::RanchReceive, DataPacket::emNormal, C2SPacketRanchReceive)
_SERIALIZE_MEMBER(ui64, referrer)
_END_PACKET

//C2SPacketRanchExpand
_BEGIN_PACKET_C2S(C2SPacketType::RanchExpand, DataPacket::emNormal, C2SPacketRanchExpand)
_SERIALIZE_MEMBER(i32, landCode)
_END_PACKET

//C2SPacketRanchShortcut
_BEGIN_PACKET_C2S(C2SPacketType::RanchShortcut, DataPacket::emNormal, C2SPacketRanchShortcut)
_SERIALIZE_MEMBER(i32, landCode)
_SERIALIZE_MEMBER(i32, moneyType)
_END_PACKET

//C2SPacketRanchDestroyHouse
_BEGIN_PACKET_C2S(C2SPacketType::RanchDestroyHouse, DataPacket::emNormal, C2SPacketRanchDestroyHouse)
_END_PACKET

//C2SPacketRanchSellItem
_BEGIN_PACKET_C2S(C2SPacketType::RanchSellItem, DataPacket::emNormal, C2SPacketRanchSellItem)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, num)
_END_PACKET

//C2SPacketRanchBuyItems
_BEGIN_PACKET_C2S(C2SPacketType::RanchBuyItems, DataPacket::emNormal, C2SPacketRanchBuyItems)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, items)
_END_PACKET

//C2SPacketRanchBuildTakeProduct
_BEGIN_PACKET_C2S(C2SPacketType::RanchBuildTakeProduct, DataPacket::emNormal, C2SPacketRanchBuildTakeProduct)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, index)
_END_PACKET

//C2SPacketRanchBuildQueueOperation
_BEGIN_PACKET_C2S(C2SPacketType::RanchBuildQueueOperation, DataPacket::emNormal, C2SPacketRanchBuildQueueOperation)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, queueId)
_SERIALIZE_MEMBER(i32, productId)
_END_PACKET

//C2SPacketRanchBuildRemove
_BEGIN_PACKET_C2S(C2SPacketType::RanchBuildRemove, DataPacket::emNormal, C2SPacketRanchBuildRemove)
_SERIALIZE_MEMBER(i32, actorId)
_END_PACKET
//C2SPacketRanchOrderOperate
_BEGIN_PACKET_C2S(C2SPacketType::RanchOrderOperate, DataPacket::emNormal, C2SPacketRanchOrderOperate)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, orderId)
_SERIALIZE_MEMBER(i32, index)
_END_PACKET

//C2SPacketRancherExBegin
_BEGIN_PACKET_C2S(C2SPacketType::RanchExBegin, DataPacket::emNormal, C2SPacketRancherExBegin)
_END_PACKET

//C2SPacketRancherExTaskFinish
_BEGIN_PACKET_C2S(C2SPacketType::RanchExTaskFinish, DataPacket::emNormal, C2SPacketRancherExTaskFinish)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, itemNum)
_END_PACKET

//C2SPacketRanchGiftsOrInvite
_BEGIN_PACKET_C2S(C2SPacketType::RanchGiftsOrInvite, DataPacket::emNormal, C2SPacketRanchGiftsOrInvite)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i64, targetUserId)
_END_PACKET

//C2SPacketRanchHelpFinish
_BEGIN_PACKET_C2S(C2SPacketType::RanchHelpFinish, DataPacket::emNormal, C2SPacketRanchHelpFinish)
_SERIALIZE_MEMBER(i64, type)
_SERIALIZE_MEMBER(i64, helpId)
_END_PACKET

//C2SPacketReceiveMailReward
_BEGIN_PACKET_C2S(C2SPacketType::ReceiveMailReward, DataPacket::emNormal, C2SPacketReceiveMailReward)
_SERIALIZE_MEMBER(i64, mailId)
_END_PACKET

//C2SPacketReceiveMailReward
_BEGIN_PACKET_C2S(C2SPacketType::ReceiveRanchAchievementReward, DataPacket::emNormal, C2SPacketReceiveRanchAchievementReward)
_SERIALIZE_MEMBER(i64, achievementId)
_END_PACKET

//C2SPacketReceiveMailReward
_BEGIN_PACKET_C2S(C2SPacketType::RanchGoExplore, DataPacket::emNormal, C2SPacketRanchGoExplore)
_END_PACKET

//C2SPacketChangeCurrentItemInfo
_BEGIN_PACKET_C2S(C2SPacketType::ChangeCurrentItemInfo, DataPacket::emNormal, C2SPacketChangeCurrentItemInfo)
_SERIALIZE_MEMBER(i32, itemIndex)
_END_PACKET

//C2SPacketSpeedUpCrop
_BEGIN_PACKET_C2S(C2SPacketType::SpeedUpCrop, DataPacket::emNormal, C2SPacketSpeedUpCrop)
_SERIALIZE_MEMBER(Vector3i, blockPos)
_SERIALIZE_MEMBER(i32, blockId)
_END_PACKET

//C2SPacketSpeedUpCrop
_BEGIN_PACKET_C2S(C2SPacketType::UseCubeFinshOrder, DataPacket::emNormal, C2SPacketUseCubeFinshOrder)
_SERIALIZE_MEMBER(i32, orderId)
_SERIALIZE_MEMBER(i32, index)
_END_PACKET

//C2SPacketBuyFlyingPermissionResult
_BEGIN_PACKET_C2S(C2SPacketType::BuyFlyingPermissionResult, DataPacket::emNormal, C2SPacketBuyFlyingPermissionResult)
_SERIALIZE_MEMBER(bool, isSuccessful)
_END_PACKET

//C2SPacketResetCloseup
_BEGIN_PACKET_C2S(C2SPacketType::ResetCloseup, DataPacket::emNormal, C2SPacketResetCloseup)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, playerPos)
_END_PACKET

//C2SPacketKeepItem
_BEGIN_PACKET_C2S(C2SPacketType::KeepItem, DataPacket::emNormal, C2SPacketKeepItem)
_SERIALIZE_MEMBER(bool, isKeepItem)
_END_PACKET

//C2SPacketOpenEnchantMent
_BEGIN_PACKET_C2S(C2SPacketType::OpenEnchantMent, DataPacket::emNormal, C2SPacketOpenEnchantMent)
_END_PACKET

//C2SPacketEnchantmentEffectClick
_BEGIN_PACKET_C2S(C2SPacketType::EnchantmentEffectClick, DataPacket::emNormal, C2SPacketEnchantmentEffectClick)
_SERIALIZE_MEMBER(i32, equipId)
_SERIALIZE_MEMBER(i32, consumeId)
_SERIALIZE_MEMBER(i32, index)
_END_PACKET

//C2SPacketEnchantmentQuick
_BEGIN_PACKET_C2S(C2SPacketType::EnchantmentQuick, DataPacket::emNormal, C2SPacketEnchantmentQuick)
_SERIALIZE_MEMBER(i32, equipId)
_SERIALIZE_MEMBER(i32, effectIndex)
_END_PACKET


//C2SPacketSelectLottery
_BEGIN_PACKET_C2S(C2SPacketType::SelectLottery, DataPacket::emNormal, C2SPacketSelectLottery)
_SERIALIZE_MEMBER(String, uniqueId)
_END_PACKET

//C2SPacketLotteryAgain
_BEGIN_PACKET_C2S(C2SPacketType::LotteryAgain, DataPacket::emNormal, C2SPacketLotteryAgain)
_END_PACKET

//C2SPacketHallLotteryAgain
_BEGIN_PACKET_C2S(C2SPacketType::HallLotteryAgain, DataPacket::emNormal, C2SPacketHallLotteryAgain)
_SERIALIZE_MEMBER(int, luck_times)
_SERIALIZE_MEMBER(int, entityId)
_END_PACKET

//C2SPacketSetSpYaw
_BEGIN_PACKET_C2S(C2SPacketType::SetSpYaw, DataPacket::emNormal, C2SPacketSetSpYaw)
_SERIALIZE_MEMBER(bool8, isSpYaw)
_SERIALIZE_MEMBER(float, radian)
_END_PACKET

//C2SPacketOpenSelectMode
_BEGIN_PACKET_C2S(C2SPacketType::OpenSelectMode, DataPacket::emNormal, C2SPacketOpenSelectMode)
_END_PACKET

//C2SPacketPixelHallUnlockMap
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunHallUnlockMap, DataPacket::emNormal, C2SPacketPixelHallUnlockMap)
_SERIALIZE_MEMBER(i32, num)
_SERIALIZE_MEMBER(i32, mapId)
_END_PACKET

//C2SPacketUseStoreGun
_BEGIN_PACKET_C2S(C2SPacketType::UseStoreGun, DataPacket::emNormal, C2SPacketUseStoreGun)
_SERIALIZE_MEMBER(int, gunId)
_END_PACKET

//C2SPacketBuyStoreGun
_BEGIN_PACKET_C2S(C2SPacketType::BuyStoreGun, DataPacket::emNormal, C2SPacketBuyStoreGun)
_SERIALIZE_MEMBER(int, gunId)
_END_PACKET

//C2SPacketUpgradeStoreGun
_BEGIN_PACKET_C2S(C2SPacketType::UpgradeStoreGun, DataPacket::emNormal, C2SPacketUpgradeStoreGun)
_SERIALIZE_MEMBER(int, gunId)
_END_PACKET

//C2SPacketQuickUpgradeStoreGun
_BEGIN_PACKET_C2S(C2SPacketType::QuickUpgradeStoreGun, DataPacket::emNormal, C2SPacketQuickUpgradeStoreGun)
_SERIALIZE_MEMBER(int, gunId)
_END_PACKET

//C2SPacketUseStoreProp
_BEGIN_PACKET_C2S(C2SPacketType::UseStoreProp, DataPacket::emNormal, C2SPacketUseStoreProp)
_SERIALIZE_MEMBER(int, propId)
_END_PACKET

//C2SPacketUnloadStoreProp
_BEGIN_PACKET_C2S(C2SPacketType::UnloadStoreProp, DataPacket::emNormal, C2SPacketUnloadStoreProp)
_SERIALIZE_MEMBER(int, propId)
_END_PACKET

//C2SPacketBuyStoreProp
_BEGIN_PACKET_C2S(C2SPacketType::BuyStoreProp, DataPacket::emNormal, C2SPacketBuyStoreProp)
_SERIALIZE_MEMBER(int, propId)
_END_PACKET

//C2SPacketUpgradeStoreProp
_BEGIN_PACKET_C2S(C2SPacketType::UpgradeStoreProp, DataPacket::emNormal, C2SPacketUpgradeStoreProp)
_SERIALIZE_MEMBER(int, propId)
_END_PACKET

//C2SPacketQuickUpgradeStoreProp
_BEGIN_PACKET_C2S(C2SPacketType::QuickUpgradeStoreProp, DataPacket::emNormal, C2SPacketQuickUpgradeStoreProp)
_SERIALIZE_MEMBER(int, propId)
_END_PACKET

//C2SPacketEffectGunFire
_BEGIN_PACKET_C2S(C2SPacketType::EffectGunFire, DataPacket::emNormal, C2SPacketEffectGunFire)
_SERIALIZE_MEMBER(bool, isNeedConsumeBullet)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, beginPos)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, endPos)
_END_PACKET

//C2SPacketEffectGunFireResult
_BEGIN_PACKET_C2S(C2SPacketType::EffectGunFireResult, DataPacket::emNormal, C2SPacketEffectGunFireResult)
_SERIALIZE_MEMBER(ui32, hitEntityId)
_SERIALIZE_MEMBER(ui32, gunId)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3i, blockPos)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, dir)
_SERIALIZE_MEMBER(ui32, traceType)
_SERIALIZE_MEMBER(bool, isHeadshot)
_END_PACKET

//C2SPacketEffectGunFireExplosionResult
_BEGIN_PACKET_C2S(C2SPacketType::EffectGunFireExplosionResult, DataPacket::emNormal, C2SPacketEffectGunFireExplosionResult)
_SERIALIZE_MEMBER(C2SPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(float, explosionRange)
_END_PACKET

//C2SPacketStopLaserGun
_BEGIN_PACKET_C2S(C2SPacketType::StopLaserGun, DataPacket::emNormal, C2SPacketStopLaserGun)
_END_PACKET

//C2SPacketFirstShowStoreGun
_BEGIN_PACKET_C2S(C2SPacketType::FirstShowStoreGun, DataPacket::emNormal, C2SPacketFirstShowStoreGun)
_SERIALIZE_MEMBER(int, gunId)
_END_PACKET

//C2SPacketFirstShowStoreProp
_BEGIN_PACKET_C2S(C2SPacketType::FirstShowStoreProp, DataPacket::emNormal, C2SPacketFirstShowStoreProp)
_SERIALIZE_MEMBER(int, propId)
_END_PACKET

//C2SPacketPixelGunSelectBox
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunSelectBox, DataPacket::emNormal, C2SPacketPixelGunSelectBox)
_SERIALIZE_MEMBER(int, index)
_END_PACKET

//C2SPacketPixelGunExit
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunExit, DataPacket::emNormal, C2SPacketPixelGunExit)
_END_PACKET

//C2SPacketPixelGunRematch
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunRematch, DataPacket::emNormal, C2SPacketPixelGunRematch)
_END_PACKET

//C2SPacketPixelGunRevive
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunRevive, DataPacket::emNormal, C2SPacketPixelGunRevive)
_END_PACKET

//C2SPacketOpenLotteryChest
_BEGIN_PACKET_C2S(C2SPacketType::OpenLotteryChest, DataPacket::emNormal, C2SPacketOpenLotteryChest)
_SERIALIZE_MEMBER(int, chestId)
_END_PACKET

//C2SPacketOpenArmorUpgrade
_BEGIN_PACKET_C2S(C2SPacketType::OpenArmorUpgrade, DataPacket::emNormal, C2SPacketOpenArmorUpgrade)
_END_PACKET

//C2SPacketUpgradeArmor
_BEGIN_PACKET_C2S(C2SPacketType::UpgradeArmor, DataPacket::emNormal, C2SPacketUpgradeArmor)
_SERIALIZE_MEMBER(bool, state)
_SERIALIZE_MEMBER(int, operateType)
_END_PACKET

//C2SPacketPixelGunResultBack
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunResultBack, DataPacket::emNormal, C2SPacketPixelGunResultBack)
_END_PACKET

//C2SPacketPixelGunResultNext
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunResultNext, DataPacket::emNormal, C2SPacketPixelGunResultNext)
_END_PACKET

//C2SPacketPixelGunResultRevenge
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunResultRevenge, DataPacket::emNormal, C2SPacketPixelGunResultRevenge)
_END_PACKET

//C2SPacketPixelGunFight
_BEGIN_PACKET_C2S(C2SPacketType::PixelGunFight, DataPacket::emNormal, C2SPacketPixelGunFight)
_SERIALIZE_MEMBER(String, gameType)
_SERIALIZE_MEMBER(String, mapName)
_END_PACKET

//C2SPacketConsumeBullet
_BEGIN_PACKET_C2S(C2SPacketType::ConsumeBullet, DataPacket::emNormal, C2SPacketConsumeBullet)
_END_PACKET

//C2SPacketBirdOperation
_BEGIN_PACKET_C2S(C2SPacketType::BirdBagOperation, DataPacket::emNormal, C2SPacketBirdBagOperation)
_SERIALIZE_MEMBER(i64, birdId)
_SERIALIZE_MEMBER(i32, operationType)
_END_PACKET

//C2SPacketBirdStoreOperation
_BEGIN_PACKET_C2S(C2SPacketType::BirdStoreOperation, DataPacket::emNormal, C2SPacketBirdStoreOperation)
_SERIALIZE_MEMBER(i32, storeId)
_SERIALIZE_MEMBER(i32, goodsId)
_SERIALIZE_MEMBER(i32, operationType)
_END_PACKET

//C2SPacketBirdPersonalStoreBuyGoods
_BEGIN_PACKET_C2S(C2SPacketType::BirdPersonalStoreBuyGoods, DataPacket::emNormal, C2SPacketBirdPersonalStoreBuyGoods)
_SERIALIZE_MEMBER(i32, tabId)
_SERIALIZE_MEMBER(i32, goodsId)
_END_PACKET

//C2SPacketBirdFuse
_BEGIN_PACKET_C2S(C2SPacketType::BirdFuse, DataPacket::emNormal, C2SPacketBirdFuse)
_SERIALIZE_MEMBER(vector<i64>::type, birdIds)
_END_PACKET

//C2SPacketBirdOpenTreasureChest
_BEGIN_PACKET_C2S(C2SPacketType::BirdOpenTreasureChest, DataPacket::emNormal, C2SPacketBirdOpenTreasureChest)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//C2SPacketBirdLottery
_BEGIN_PACKET_C2S(C2SPacketType::BirdLottery, DataPacket::emNormal, C2SPacketBirdLottery)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//C2SPacketBirdLottery
_BEGIN_PACKET_C2S(C2SPacketType::BirdTask, DataPacket::emNormal, C2SPacketBirdTask)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, taskId)
_END_PACKET

//C2SPacketBirdFeed
_BEGIN_PACKET_C2S(C2SPacketType::BirdFeed, DataPacket::emNormal, C2SPacketBirdFeed)
_SERIALIZE_MEMBER(i64, birdId)
_SERIALIZE_MEMBER(i32, foodId)
_SERIALIZE_MEMBER(i32, num)
_END_PACKET

//C2SPacketBirdSetDress
_BEGIN_PACKET_C2S(C2SPacketType::BirdSetDress, DataPacket::emNormal, C2SPacketBirdSetDress)
_SERIALIZE_MEMBER(i64, birdId)
_SERIALIZE_MEMBER(i32, dressType)
_SERIALIZE_MEMBER(i32, dressId)
_END_PACKET

#endif

