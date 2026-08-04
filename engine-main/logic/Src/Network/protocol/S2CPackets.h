#ifndef __S2C_PACKETS_H__
#define __S2C_PACKETS_H__

#include <array>
#include "Network/protocol/PacketDefine.h"
#include "Network/NetworkDefine.h"
#include "Entity/Entity.h"
#include "Entity/EntityXPOrb.h"
#include "Entity/EntityMob.h"
#include "Entity/EntityPlayer.h"
#include "Entity/EntityItem.h"
#include "Item/Potion.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Chat/ChatComponent.h"
#include "Entity/EntityMerchant.h"
#include "World/Shop.h"
#include "Setting/LogicSetting.h"
#include <list>
#include "World/Manor.h"
#include "Entity/EntityActorCannon.h"
#include "World/Ranch.h"
#include "World/BirdSimulator.h"
#include "Entity/EntityBuildNpc.h"
#include "Item/Item.h"

using namespace LORD;

namespace S2CPACKET_DETAIL
{
	using Vector3 = std::array<real32, 3>;
	using Vector3i = std::array<i32, 3>;
}

// S2CPacketAddPlayer
_BEGIN_PACKET_S2C(S2CPacketType::AddPlayer, DataPacket::emNormal, S2CPacketAddPlayer)
_SERIALIZE_MEMBER(i32, m_runtimePlayer)
_SERIALIZE_MEMBER(String, m_playerName)
_END_PACKET

// S2CPacketDebugString
_BEGIN_PACKET_S2C(S2CPacketType::DebugString, DataPacket::emNormal, S2CPacketDebugString)
_SERIALIZE_MEMBER(String, debugString)
_END_PACKET

//S2CPacketUserAttr
_BEGIN_PACKET_S2C(S2CPacketType::UserAttr, DataPacket::emNormal, S2CPacketUserAttr)

public:
	struct SkinColor
	{
		Real r=0, g=0, b=0, a=0;
		SkinColor() {
		}
		SkinColor(Real fr, Real fg, Real fb, Real fa = 0.f)
		{
			r = fr;
			g = fg;
			b = fb;
			a = fa;
		}
		DEFINE_NETWORK_SERIALIZER(r, g, b, a);
	};
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, faceId)
_SERIALIZE_MEMBER(i32, hairId)
_SERIALIZE_MEMBER(i32, topsId)
_SERIALIZE_MEMBER(i32, pantsId)
_SERIALIZE_MEMBER(i32, shoesId)
_SERIALIZE_MEMBER(i32, glassesId)
_SERIALIZE_MEMBER(i32, scarfId)
_SERIALIZE_MEMBER(i32, wingId)
_SERIALIZE_MEMBER(i32, hatId)
_SERIALIZE_MEMBER(i32, decoratehatId)
_SERIALIZE_MEMBER(i32, armId)
_SERIALIZE_MEMBER(i32, tailId)
_SERIALIZE_MEMBER(i32, extrawingId)
_SERIALIZE_MEMBER(i32, footHaloId)
_SERIALIZE_MEMBER(i32, backEffectId)
_SERIALIZE_MEMBER(i32, crownId)
_SERIALIZE_MEMBER(i32, bagId)
_SERIALIZE_MEMBER(i32, headEffectId)
_SERIALIZE_MEMBER(SkinColor, skinColor)
_END_PACKET

//S2CPacketMovePlayer
_BEGIN_PACKET_S2C(S2CPacketType::MovePlayer, DataPacket::emNormal, S2CPacketMovePlayer)
_SERIALIZE_MEMBER(i32, m_playerRuntimeId)

_SERIALIZE_MEMBER(real32, m_x)
_SERIALIZE_MEMBER(real32, m_y)
_SERIALIZE_MEMBER(real32, m_z)

_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(real32, m_headYaw)
_SERIALIZE_MEMBER(real32, m_swing)
_SERIALIZE_MEMBER(real32, m_limbYaw)
_END_PACKET


//S2CPacketAnimate
_BEGIN_PACKET_S2C(S2CPacketType::Animate, DataPacket::emNormal, S2CPacketAnimate)
_SERIALIZE_MEMBER(i32, m_entityRuntimeId)
_SERIALIZE_MEMBER(NETWORK_DEFINE::PacketAnimateType, m_animateType)
_SERIALIZE_MEMBER(i32, m_value)
_END_PACKET



//S2CPacketEntityHeadRotation
_BEGIN_PACKET_S2C(S2CPacketType::EntityHeadRotation, DataPacket::emNormal, S2CPacketEntityHeadRotation)
_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(real32, m_headYaw)
_END_PACKET

//S2CPacketEntityMovement
_BEGIN_PACKET_S2C(S2CPacketType::EntityMovement, DataPacket::emNormal, S2CPacketEntityMovement)
public:
	S2CPacket(int etyid)
		: m_entityId(etyid)
		, m_x(0)
		, m_y(0)
		, m_z(0)
		, m_yaw(0.0)
		, m_pitch(0.0)
		, m_moving(false)
		, m_rotating(false)
	{}

	S2CPacket(int etyid, i16 x, i16 y, i16 z)
		: m_entityId(etyid)
		, m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(0.0)
		, m_pitch(0.0)
		, m_moving(true)
		, m_rotating(false)
	{}

	S2CPacket(int etyid, real32 yaw, real32 pitch)
		: m_entityId(etyid)
		, m_x(0)
		, m_y(0)
		, m_z(0)
		, m_yaw(yaw)
		, m_pitch(pitch)
		, m_moving(false)
		, m_rotating(true)
	{}

	S2CPacket(int etyid, i16 x, i16 y, i16 z, real32 yaw, real32 pitch)
		: m_entityId(etyid)
		, m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(yaw)
		, m_pitch(pitch)
		, m_moving(true)
		, m_rotating(true)
	{}
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(i16, m_x)
_SERIALIZE_MEMBER(i16, m_y)
_SERIALIZE_MEMBER(i16, m_z)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(bool8, m_moving)
_SERIALIZE_MEMBER(bool8, m_rotating)
_END_PACKET

//S2CPacketEntityTeleport
_BEGIN_PACKET_S2C(S2CPacketType::EntityTeleport, DataPacket::emNormal, S2CPacketEntityTeleport)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity)
	{
		m_entityId = pEntity->entityId;
		m_x = int(Math::Floor(pEntity->position.x * 32.0f));
		m_y = int(Math::Floor(pEntity->position.y * 32.0f));
		m_z = int(Math::Floor(pEntity->position.z * 32.0f));
		// m_yaw = (i16)((int)(pEntity->rotationYaw * 256.0F / 360.0F));
		// m_pitch = (i16)((int)(pEntity->rotationPitch * 256.0F / 360.0F));
		m_yaw = pEntity->rotationYaw;
		m_pitch = pEntity->rotationPitch;
	}

	S2CPacket(int eid, int x, int y, int z, real32 yaw, real32 pitch)
		: m_entityId(eid)
		, m_x(x)
		, m_y(y)
		, m_z(z)
		, m_yaw(yaw)
		, m_pitch(pitch)
	{}
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(i32, m_x)
_SERIALIZE_MEMBER(i32, m_y)
_SERIALIZE_MEMBER(i32, m_z)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_END_PACKET

//S2CPacketEntityVelocity
_BEGIN_PACKET_S2C(S2CPacketType::EntityVelocity, DataPacket::emNormal, S2CPacketEntityVelocity)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity)
	{
		m_entityID = pEntity->entityId;
		setVolocity(pEntity->motion.x, pEntity->motion.y, pEntity->motion.z);
	}

	S2CPacket(int id, float x, float y, float z)
	{
		m_entityID = id;
		setVolocity(x, y, z);
	}

	void setVolocity(float x, float y, float z)
	{
		float bound = 3.9f;

		x = Math::Clamp(x, -bound, bound);
		y = Math::Clamp(y, -bound, bound);
		z = Math::Clamp(z, -bound, bound);

		m_motionX = (int)(x * 8000.0f);
		m_motionY = (int)(y * 8000.0f);
		m_motionZ = (int)(z * 8000.0f);
	}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(i32, m_motionX)
_SERIALIZE_MEMBER(i32, m_motionY)
_SERIALIZE_MEMBER(i32, m_motionZ)
_END_PACKET


//S2CPacketSpawnExpOrb
_BEGIN_PACKET_S2C(S2CPacketType::SpawnExpOrb, DataPacket::emNormal, S2CPacketSpawnExpOrb)
public:
	S2CPacket(BLOCKMAN::EntityXPOrb* pXpOrb)
	{
		m_entityID = pXpOrb->entityId;
		m_posX = int(Math::Floor(pXpOrb->position.x * 32.f));
		m_posY = int(Math::Floor(pXpOrb->position.y * 32.f));
		m_posZ = int(Math::Floor(pXpOrb->position.z * 32.f));
		m_xpValue = pXpOrb->getXpValue();
	}

	S2CPacket(int entityID, int x, int y, int z, int exp)
		: m_entityID(entityID)
		, m_posX(x)
		, m_posY(y)
		, m_posZ(z)
		, m_xpValue(exp)
	{}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(i32, m_posX)
_SERIALIZE_MEMBER(i32, m_posY)
_SERIALIZE_MEMBER(i32, m_posZ)
_SERIALIZE_MEMBER(i32, m_xpValue)
_END_PACKET

//S2CPacketSpawnMob
_BEGIN_PACKET_S2C(S2CPacketType::SpawnMob, DataPacket::emNormal, S2CPacketSpawnMob)
public:
	S2CPacket(BLOCKMAN::EntityLivingBase* pLiving)
	{
		m_entityID = pLiving->entityId;
		m_type = pLiving->getClassID();
		m_posX = int(Math::Floor(pLiving->position.x * 32.f));
		m_posY = int(Math::Floor(pLiving->position.y * 32.f));
		m_posZ = int(Math::Floor(pLiving->position.z * 32.f));
		m_yaw = (i16)((int)(pLiving->rotationYaw * 256.f / 360.f));
		m_pitch = (i16)((int)(pLiving->rotationPitch * 256.f / 360.f));
		m_headYaw = (i16)((int)(pLiving->rotationYawHead * 256.f / 360.f));

		float bound = 3.9f;
		Vector3 motion = pLiving->motion;
		motion.x = Math::Clamp(motion.x, -bound, bound);
		motion.y = Math::Clamp(motion.y, -bound, bound);
		motion.z = Math::Clamp(motion.z, -bound, bound);

		m_motionX = (int)(motion.x * 8000.f);
		m_motionY = (int)(motion.y * 8000.f);
		m_motionZ = (int)(motion.z * 8000.f);
	}
_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(i32, m_type)
_SERIALIZE_MEMBER(i32, m_posX)
_SERIALIZE_MEMBER(i32, m_posY)
_SERIALIZE_MEMBER(i32, m_posZ)
_SERIALIZE_MEMBER(i32, m_motionX)
_SERIALIZE_MEMBER(i32, m_motionY)
_SERIALIZE_MEMBER(i32, m_motionZ)
_SERIALIZE_MEMBER(i16, m_yaw)
_SERIALIZE_MEMBER(i16, m_pitch)
_SERIALIZE_MEMBER(i16, m_headYaw)
_END_PACKET

//S2CPacketSpawnPlayer
_BEGIN_PACKET_S2C(S2CPacketType::SpawnPlayer, DataPacket::emNormal, S2CPacketSpawnPlayer)
public:
	S2CPacket(BLOCKMAN::EntityPlayer* pPlayer)
	{
		m_entityID = pPlayer->entityId;
		m_name = pPlayer->getCommandSenderName();
		m_showName = pPlayer->getShowName();
		m_posX = int(Math::Floor(pPlayer->position.x * 32.f));
		m_posY = int(Math::Floor(pPlayer->position.y * 32.f));
		m_posZ = int(Math::Floor(pPlayer->position.z * 32.f));
		// m_yaw = (i16)((int)(pPlayer->rotationYaw * 256.0F / 360.0F));
		// m_pitch = (i16)((int)(pPlayer->rotationPitch * 256.0F / 360.0F));
		m_yaw = pPlayer->rotationYaw;
		m_pitch = pPlayer->rotationPitch;
		m_invisible = pPlayer->isInvisible();
		m_sex = pPlayer->m_sex;
		m_teamId = pPlayer->getTeamId();
		m_teamName = pPlayer->getTeamName();
		m_platformUserId = pPlayer->getPlatformUserId();
		m_defaultIdle = pPlayer->m_defaultIdle;
		m_isSpYaw = pPlayer->spYaw;
		m_SpYawRadian = pPlayer->spYawRadian;
	}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(String, m_name)
_SERIALIZE_MEMBER(String, m_showName)
_SERIALIZE_MEMBER(i32, m_posX)
_SERIALIZE_MEMBER(i32, m_posY)
_SERIALIZE_MEMBER(i32, m_posZ)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(bool8, m_invisible)
_SERIALIZE_MEMBER(i32, m_sex)
_SERIALIZE_MEMBER(i32, m_teamId)
_SERIALIZE_MEMBER(String, m_teamName)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(i32, m_defaultIdle)
_SERIALIZE_MEMBER(bool8, m_isSpYaw)
_SERIALIZE_MEMBER(real32, m_SpYawRadian)
_SERIALIZE_MEMBER(bool8, m_isNamePerspective)
_END_PACKET


//S2CEntitySpawnCommonPacket
_BEGIN_PACKET_S2C(S2CPacketType::SpawnVehicle, DataPacket::emNormal, S2CEntitySpawnCommonPacket)
public:
	S2CPacket(BLOCKMAN::EntityItem* pEntity, int type, int throwerID)
	{
		m_entityID = pEntity->entityId;
		m_itemID = pEntity->getEntityItem()->itemID;
		m_itemDamage = pEntity->getEntityItem()->itemDamage;
		m_pos = pEntity->position;
		m_pitch = pEntity->rotationPitch;
		m_yaw = pEntity->rotationYaw;
		m_type = type;
		m_count = pEntity->getEntityItem()->stackSize;
		m_throwerID = throwerID;
		auto tagList = pEntity->getEntityItem()->getEnchantmentTagList();
		m_enchanted = tagList && tagList->tagCount() > 0;
		m_onFire = pEntity->isBurning();
		m_motion = pEntity->motion;
	}

	S2CPacket(BLOCKMAN::Entity* pEntity, int type, int throwerID = 0, BLOCKMAN::PotionEffect* effect = nullptr, int meta = 0)
	{
		m_entityID = pEntity->entityId;
		m_pos = pEntity->position;
		m_pitch = pEntity->rotationPitch;
		m_yaw = pEntity->rotationYaw;
		m_type = type;
		m_throwerID = throwerID;
		m_onFire = pEntity->isBurning();
		m_motion = pEntity->motion;
		m_itemDamage = meta;

		if (effect)
		{
			potionId = effect->getPotionID();
			amplifier = effect->getAmplifier();
			duration = effect->getDuration();
			isAmbient = effect->getIsAmbient();
			isSplashPotion = effect->getIsSplashPotion();
			isPotionDurationMax = effect->getIsPotionDurationMax();
		}
	}

_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(i32, m_itemID)
_SERIALIZE_MEMBER(i32, m_itemDamage)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, m_pos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, m_motion)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(real32, m_yaw)
_SERIALIZE_MEMBER(i32, m_count)
_SERIALIZE_MEMBER(i32, m_type)
_SERIALIZE_MEMBER(i32, m_throwerID)
_SERIALIZE_MEMBER(bool8, m_enchanted)
_SERIALIZE_MEMBER(bool8, m_onFire)
_SERIALIZE_MEMBER(i32, potionId)
_SERIALIZE_MEMBER(i32, amplifier)
_SERIALIZE_MEMBER(i32, duration)
_SERIALIZE_MEMBER(bool8, isAmbient)
_SERIALIZE_MEMBER(bool8, isSplashPotion)
_SERIALIZE_MEMBER(bool8, isPotionDurationMax)
_END_PACKET 

//S2CPacketRemoveEntity
_BEGIN_PACKET_S2C(S2CPacketType::RemoveEntity, DataPacket::emNormal, S2CPacketRemoveEntity)
_SERIALIZE_MEMBER(i32, m_entityID)
_END_PACKET

//S2CPacketUpdateAttribute
_BEGIN_PACKET_S2C(S2CPacketType::UpdatePlayerAttribute, DataPacket::emNormal, S2CPacketUpdateAttribute)
public:
	S2CPacket()
		:m_entityRuntimeId(0)
		,m_health(0.0f)
		,m_air(0)
		,m_foodLevel(0)
		,m_foodSaturationLevel(0.0f)
	{

	}

	S2CPacket(BLOCKMAN::EntityPlayer * pEntity)
	{
		m_entityRuntimeId = pEntity->entityId;
		m_health = pEntity->getHealth();
		m_air = pEntity->getAir();
		m_foodLevel = pEntity->getFoodStats()->getFoodLevel();
		m_foodSaturationLevel = pEntity->getFoodStats()->getSaturationLevel();
	}
_SERIALIZE_MEMBER(i32, m_entityRuntimeId)
_SERIALIZE_MEMBER(real32, m_health)
_SERIALIZE_MEMBER(i32, m_air)
_SERIALIZE_MEMBER(i32, m_foodLevel)
_SERIALIZE_MEMBER(real32, m_foodSaturationLevel)
_END_PACKET

//S2CPacketRebirth
_BEGIN_PACKET_S2C(S2CPacketType::Rebirth, DataPacket::emNormal, S2CPacketRebirth)
_SERIALIZE_MEMBER(i32, m_playerRuntimeId)
_SERIALIZE_MEMBER(i16, m_posX)
_SERIALIZE_MEMBER(i16, m_posY)
_SERIALIZE_MEMBER(i16, m_posZ)
_SERIALIZE_MEMBER(i32, m_sex)
_SERIALIZE_MEMBER(String, m_showName)
_END_PACKET


// S2CPacketChangeItemInHand
_BEGIN_PACKET_S2C(S2CPacketType::ChangeItemInHand, DataPacket::emNormal, S2CPacketChangeItemInHand)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(bool8, enchanted)
_END_PACKET

// S2CPacketChangeCurrentItem
_BEGIN_PACKET_S2C(S2CPacketType::ChangeCurrentItem, DataPacket::emNormal, S2CPacketChangeCurrentItem)
_SERIALIZE_MEMBER(i32, itemIndex)
_END_PACKET

// S2CPacketEquipArmors
_BEGIN_PACKET_S2C(S2CPacketType::EquipArmors, DataPacket::emNormal, S2CPacketEquipArmors)
public:
	struct ArmorInfo
	{
		i32 itemId;
		bool8 enchanted;
		ArmorInfo() = default;
		ArmorInfo(BLOCKMAN::ItemStackPtr armor)
		{
			itemId = armor->itemID;
			const auto tagList = armor->getEnchantmentTagList();
			enchanted = tagList && tagList->tagCount() > 0;
		}

		DEFINE_NETWORK_SERIALIZER(itemId, enchanted);
	};
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(vector<ArmorInfo>::type, armors)
_END_PACKET

// S2CPacketUnequipArmor
_BEGIN_PACKET_S2C(S2CPacketType::UnequipArmor, DataPacket::emNormal, S2CPacketUnequipArmor)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(i32, slotIndex)
_END_PACKET

// S2CPacketDestroyArmor
_BEGIN_PACKET_S2C(S2CPacketType::DestroyArmor, DataPacket::emNormal, S2CPacketDestroyArmor)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(i32, slotIndex)
_END_PACKET

// S2CPacketPlayerAction
_BEGIN_PACKET_S2C(S2CPacketType::PlayerAction, DataPacket::emNormal, S2CPacketPlayerAction)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(NETWORK_DEFINE::PacketPlayerActionType, action)
_END_PACKET

// S2CPacketUpdateBlocks
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBlocks, DataPacket::emNormal, S2CPacketUpdateBlocks)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::S2CPacketUpdateBlockInfo>::type, updates)
_END_PACKET

// S2CPacketChatString
_BEGIN_PACKET_S2C(S2CPacketType::ChatMessageString, DataPacket::emNormal, S2CPacketChatString)
_SERIALIZE_MEMBER(i32, m_chatType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentString>, m_chatComponent)
_END_PACKET

// S2CPacketChatTranslation
_BEGIN_PACKET_S2C(S2CPacketType::ChatMessageTranslation, DataPacket::emNormal, S2CPacketChatTranslation)
_SERIALIZE_MEMBER(i32, m_chatType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentTranslation>, m_chatComponent)
_END_PACKET

// ChatComponentNotification
_BEGIN_PACKET_S2C(S2CPacketType::ChatMessageNotification, DataPacket::emNormal, S2CPacketChatNotification)
_SERIALIZE_MEMBER(i32, m_chatType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentNotification>, m_chatComponent)
_END_PACKET

// GameTipNotification
_BEGIN_PACKET_S2C(S2CPacketType::GameTipNotification, DataPacket::emNormal, S2CPacketGameTipNotification)
_SERIALIZE_MEMBER(i32, m_tipType)
_SERIALIZE_MEMBER(std::shared_ptr<ChatComponentNotification>, m_chatComponent)
_END_PACKET

namespace NETWORK_DEFINE
{

	struct EnchantmentInfo
	{
		i16 id;
		i16 level;
		DEFINE_NETWORK_SERIALIZER(id, level);
	};

	struct ItemStackInfo
	{
		bool8 valid;
		i32 id;
		i32 size;
		i32 damage;
		i32 curClipBullet;
		i32 totalBullet;
		i32 index;
		i32 maxDamage;
		bool8 needAdjustStackIndex;
		vector<EnchantmentInfo>::type enchantments;
		DEFINE_NETWORK_SERIALIZER(valid, id, size, damage, curClipBullet, totalBullet, index, maxDamage, needAdjustStackIndex, enchantments);

		ItemStackInfo() = default;
		ItemStackInfo(std::shared_ptr<BLOCKMAN::ItemStack> itemStack, i32 inventoryIndex)
		{
			index = inventoryIndex;
			if (!itemStack)
			{
				valid = false;
				id = 0;
				size = 0;
				damage = 0;
				curClipBullet = 0;
				totalBullet = 0;
				maxDamage = 0;
				needAdjustStackIndex = true;
				return;
			}
			valid = true;
			id = itemStack->itemID;
			size = itemStack->stackSize;
			damage = itemStack->itemDamage;
			curClipBullet = itemStack->getCurrentClipBulletNum();
			totalBullet = itemStack->getTotalBulletNum();
			needAdjustStackIndex = itemStack->m_needAdjustStackIndex;

			BLOCKMAN::Item* pItem = BLOCKMAN::Item::getItemById(id);
			if (pItem)
				maxDamage = pItem->getMaxDamage();

			auto tagList = itemStack->getEnchantmentTagList();
			if (!tagList)
				return;
			for (int i = 0; i < tagList->tagCount(); ++i)
			{
				EnchantmentInfo enchantmentInfo;
				enchantmentInfo.id = ((BLOCKMAN::NBTTagCompound*)tagList->tagAt(i))->getShort("id");
				enchantmentInfo.level = ((BLOCKMAN::NBTTagCompound*)tagList->tagAt(i))->getShort("lvl");
				enchantments.push_back(enchantmentInfo);
			}
		}
	};

	struct CoinMapping
	{
		i32 coinId;
		i32 itemId;
		DEFINE_NETWORK_SERIALIZER(coinId, itemId);

		CoinMapping() = default;
		CoinMapping(int coinId, int itemId)
		{
			this->coinId = coinId;
			this->itemId = itemId;
		}
	};
}

//S2CPacketGameInfo
_BEGIN_PACKET_S2C(S2CPacketType::GameInfo, DataPacket::emNormal, S2CPacketGameInfo)
_SERIALIZE_MEMBER(i32, m_playerRuntimeId)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3i, spawnPos)
_SERIALIZE_MEMBER(vector<String>::type, recipes)
_SERIALIZE_MEMBER(bool8, isTimeStopped)
_SERIALIZE_MEMBER(i32, sex)
_SERIALIZE_MEMBER(String, m_gameType)
_SERIALIZE_MEMBER(i32, m_defaultIdle)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::CoinMapping>::type, m_coinMapping)
_END_PACKET

// S2CPacketPlayerInventory
_BEGIN_PACKET_S2C(S2CPacketType::PlayerInventory, DataPacket::emNormal, S2CPacketPlayerInventory)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::ItemStackInfo>::type, playerInventory)
_END_PACKET


// S2CPacketAddEntityEffect
_BEGIN_PACKET_S2C(S2CPacketType::AddEntityEffect, DataPacket::emNormal, S2CPacketAddEntityEffect)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, potionId)
_SERIALIZE_MEMBER(i32, amplifier)
_SERIALIZE_MEMBER(i32, duration)
_SERIALIZE_MEMBER(bool8, isAmbient)
_SERIALIZE_MEMBER(bool8, isSplashPotion)
_SERIALIZE_MEMBER(bool8, isPotionDurationMax)
_END_PACKET

// S2CPacketRemoveEntityEffect
_BEGIN_PACKET_S2C(S2CPacketType::RemoveEntityEffect, DataPacket::emNormal, S2CPacketRemoveEntityEffect)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, potionId)
_END_PACKET


// S2CPacketChestInventory
_BEGIN_PACKET_S2C(S2CPacketType::ChestInventory, DataPacket::emNormal, S2CPacketChestInventory)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, face)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::ItemStackInfo>::type, chestInventory)
_END_PACKET


// S2CPacketUsePropResults
_BEGIN_PACKET_S2C(S2CPacketType::UsePropResults, DataPacket::emNormal, S2CPacketUsePropResults)
_SERIALIZE_MEMBER(String, m_propId)
_SERIALIZE_MEMBER(i32, m_results)
_END_PACKET


// S2CPacketUserIn
_BEGIN_PACKET_S2C(S2CPacketType::UserIn, DataPacket::emNormal, S2CPacketUserIn)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(String, m_nickName)
_SERIALIZE_MEMBER(i32, m_teamId)
_SERIALIZE_MEMBER(String, m_teamName)
_SERIALIZE_MEMBER(i32, m_curPlayer)
_SERIALIZE_MEMBER(i32, m_maxPlayer)
_END_PACKET

// S2CPacketUserOut
_BEGIN_PACKET_S2C(S2CPacketType::UserOut, DataPacket::emNormal, S2CPacketUserOut)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_END_PACKET

// S2CPacketLoginResult
_BEGIN_PACKET_S2C(S2CPacketType::LoginFailure, DataPacket::emNormal, S2CPacketLoginResult)
_SERIALIZE_MEMBER(i32, m_resultCode)
_END_PACKET

// S2CPacketPing
_BEGIN_PACKET_S2C(S2CPacketType::Ping, DataPacket::emNormal, S2CPacketPing)
_SERIALIZE_MEMBER(ui32, m_sendTime)
_END_PACKET

// S2CPacketSyncWorldTime
_BEGIN_PACKET_S2C(S2CPacketType::SyncWorldTime, DataPacket::emNormal, S2CPacketSyncWorldTime)
_SERIALIZE_MEMBER(i64, time)
_END_PACKET

// S2CPacketSettlement
_BEGIN_PACKET_S2C(S2CPacketType::Settlement, DataPacket::emNormal, S2CPacketSettlement)
_SERIALIZE_MEMBER(String, m_gameType)
_SERIALIZE_MEMBER(String, m_gameResult)
_END_PACKET

// S2CPacketGameover
_BEGIN_PACKET_S2C(S2CPacketType::Gameover, DataPacket::emNormal, S2CPacketGameover)
_SERIALIZE_MEMBER(String, m_overMessage)
_SERIALIZE_MEMBER(i32, m_code)
_END_PACKET

// S2CPacketCloseContainer
_BEGIN_PACKET_S2C(S2CPacketType::CloseContainer, DataPacket::emNormal, S2CPacketCloseContainer)
_SERIALIZE_MEMBER(bool8, m_close)
_SERIALIZE_MEMBER(i32, m_playerID)
_SERIALIZE_MEMBER(i16, m_containerPosX)
_SERIALIZE_MEMBER(i16, m_containerPosY)
_SERIALIZE_MEMBER(i16, m_containerPosZ)
_END_PACKET


_BEGIN_PACKET_S2C(S2CPacketType::PlaySound, DataPacket::emNormal, S2CPacketPlaySound)
_SERIALIZE_MEMBER(uint, m_soundType)
_SERIALIZE_MEMBER(i32, m_entityId)
//_SERIALIZE_MEMBER(String, m_soundName)
//_SERIALIZE_MEMBER(real32, m_volumn)
//_SERIALIZE_MEMBER(real32, m_pitch)
_END_PACKET


_BEGIN_PACKET_S2C(S2CPacketType::Explosion, DataPacket::emNormal, S2CPacketExplosion)
_SERIALIZE_MEMBER(real32, m_size)
_SERIALIZE_MEMBER(vector<S2CPACKET_DETAIL::Vector3i>::type, m_poss)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, m_pos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, m_motion)
_SERIALIZE_MEMBER(real32, m_customsize)
_SERIALIZE_MEMBER(bool8, m_smoke)
_END_PACKET

//S2CPacketSetAllowFlying
_BEGIN_PACKET_S2C(S2CPacketType::SetAllowFlying, DataPacket::emNormal, S2CPacketSetAllowFlying)
_SERIALIZE_MEMBER(bool8, allowFlying)
_END_PACKET


_BEGIN_PACKET_S2C(S2CPacketType::SetSignTexts, DataPacket::emNormal, S2CPacketSetSignTexts)
public:
	struct SignData
	{
		S2CPACKET_DETAIL::Vector3i position;
		std::array<String, 4> texts;
		DEFINE_NETWORK_SERIALIZER(position, texts);
	};
_SERIALIZE_MEMBER(vector<SignData>::type, signData)
_END_PACKET


_BEGIN_PACKET_S2C(S2CPacketType::ChangePlayerInfo, DataPacket::emNormal, S2CPacketChangePlayerInfo)
_SERIALIZE_MEMBER(i32, m_playerId)
_SERIALIZE_MEMBER(String, m_playerShowName)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::GameStatus, DataPacket::emNormal, S2CPacketGameStatus)
_SERIALIZE_MEMBER(i32, m_status)
_END_PACKET

//S2CPacketSetWatchMode
_BEGIN_PACKET_S2C(S2CPacketType::SetWatchMode, DataPacket::emNormal, S2CPacketSetWatchMode)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(bool8, isWatchMode)
_END_PACKET


//S2CPacketUpdateMerchant
_BEGIN_PACKET_S2C(S2CPacketType::UpdateMerchant, DataPacket::emNormal, S2CPacketUpdateMerchant)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(vector<BLOCKMAN::CommodityGroup>::type, commodities)
_END_PACKET


//S2CPacketSetFlying
_BEGIN_PACKET_S2C(S2CPacketType::SetFlying, DataPacket::emNormal, S2CPacketSetFlying)
_SERIALIZE_MEMBER(i32, playerId)
_SERIALIZE_MEMBER(bool8, isFlying)
_END_PACKET

// S2CPacketPlayerSettlement
_BEGIN_PACKET_S2C(S2CPacketType::PlayerSettlement, DataPacket::emNormal, S2CPacketPlayerSettlement)
_SERIALIZE_MEMBER(String, m_result)
_SERIALIZE_MEMBER(bool8, m_isNextServer)
_END_PACKET

// S2CPacketGameSettlement
_BEGIN_PACKET_S2C(S2CPacketType::GameSettlement, DataPacket::emNormal, S2CPacketGameSettlement)
_SERIALIZE_MEMBER(String, m_result)
_SERIALIZE_MEMBER(bool8, m_isNextServer)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::PlayerLifeStatus, DataPacket::emNormal, S2CPacketPlayerLifeStatus)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(bool8, m_isLife)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncWorldWeather, DataPacket::emNormal, S2CPacketSyncWorldWeather)
_SERIALIZE_MEMBER(String, weather)
_SERIALIZE_MEMBER(i32, time)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::BedDestroy, DataPacket::emNormal, S2CPacketBedDestroy)
_SERIALIZE_MEMBER(i32, m_teamId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::CheckCSVersionResult, DataPacket::emNormal, S2CPacketCheckCSVersionResult)
_SERIALIZE_MEMBER(bool8, m_success)
_SERIALIZE_MEMBER(int, m_serverVersion)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::RespawnCountdown, DataPacket::emNormal, S2CPacketRespawnCountdown)
_SERIALIZE_MEMBER(i32, m_second)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::AllPlayerTeamInfo, DataPacket::emNormal, S2CPacketAllPlayerTeamInfo)
_SERIALIZE_MEMBER(String, m_result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ChangePlayerTeam, DataPacket::emNormal, S2CPacketChangePlayerTeam)
_SERIALIZE_MEMBER(ui64, m_platformUserId)
_SERIALIZE_MEMBER(i32, m_teamId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::PlayerChangeActor, DataPacket::emNormal, S2CPacketPlayerChangeActor)
_SERIALIZE_MEMBER(int, m_entityId)
_SERIALIZE_MEMBER(String, m_actorName)
_SERIALIZE_MEMBER(String, m_bodyId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::PlayerRestoreActor, DataPacket::emNormal, S2CPacketPlayerRestoreActor)
_SERIALIZE_MEMBER(int, m_entityId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ReloadBullet, DataPacket::emNormal, S2CPacketReloadBullet)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateShop, DataPacket::emNormal, S2CPacketUpdateShop)
_SERIALIZE_MEMBER(vector<BLOCKMAN::GoodsGroup>::type, m_goods)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RespawnGoods>::type, m_respawnGoods)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateShopGoods, DataPacket::emNormal, S2CPacketUpdateShopGoods)
_SERIALIZE_MEMBER(i32, groupIndex)
_SERIALIZE_MEMBER(i32, goodsIndex)
_SERIALIZE_MEMBER(i32, limit)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ShoppingResult, DataPacket::emNormal, S2CPacketShoppingResult)
_SERIALIZE_MEMBER(i32, code)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::InitMoney, DataPacket::emNormal, S2CPacketInitMoney)
_SERIALIZE_MEMBER(i64, diamonds)
_SERIALIZE_MEMBER(i64, golds)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncGunFireResult, DataPacket::emNormal, S2CPacketSyncGunFireResult)
_SERIALIZE_MEMBER(ui32, sourceEntityId)					// where the gun fires comes from
_SERIALIZE_MEMBER(ui32, gunId)							// which type gun it is
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, pos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, dir)
_SERIALIZE_MEMBER(ui32, tracyType)						// 0: none  1: block  2: entity(Player)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3i, blockPos)	// if tracyType is 1, the blockPos is the collision Block!
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, hitpos)	// if tracyType is 1 or 2, the hitpos is the collision position!
_SERIALIZE_MEMBER(ui32, hitEntityID)					// if tracyType is 2, the hitEntityID is serverID
_SERIALIZE_MEMBER(bool, isHeadshot)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncTakeVehicle, DataPacket::emNormal, S2CPacketSyncTakeVehicle)
_SERIALIZE_MEMBER(ui32, playerId)					
_SERIALIZE_MEMBER(bool8, onoff)
_SERIALIZE_MEMBER(ui32, vehicleId)
_SERIALIZE_MEMBER(bool8, isDriver)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ChangeMaxHealth, DataPacket::emNormal, S2CPacketChangeMaxHealth)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, health)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateCurrency, DataPacket::emNormal, S2CPacketUpdateCurrency)
_SERIALIZE_MEMBER(i64, currency)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SelectRoleData, DataPacket::emNormal, S2CPacketSelectRoleData)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerOwnVehicle, DataPacket::emNormal, S2CPacketSyncPlayerOwnVehicle)
_SERIALIZE_MEMBER(vector<int>::type, m_vehicles)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncLockVehicle, DataPacket::emNormal, S2CPacketSyncLockVehicle)
_SERIALIZE_MEMBER(ui32, playerId)
_SERIALIZE_MEMBER(bool8, onoff)
_SERIALIZE_MEMBER(ui32, vehicleId)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncVehicleState, DataPacket::emNormal, S2CPacketSyncVehicleState)
_SERIALIZE_MEMBER(bool8, isLocked)
_SERIALIZE_MEMBER(bool8, hasDriver)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, driverId)
_SERIALIZE_MEMBER(i32, state)
_SERIALIZE_MEMBER(real32, currentVelocity)
_SERIALIZE_MEMBER(vector<i32>::type, m_passengers)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncVehicleAction, DataPacket::emNormal, S2CPacketSyncVehicleActionState)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, state)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::NotifyGetMoney, DataPacket::emNormal, S2CPacketNotifyGetMoney)
_SERIALIZE_MEMBER(i32, count)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::NotifyGetItem, DataPacket::emNormal, S2CPacketNotifyGetItem)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_SERIALIZE_MEMBER(i32, count)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::BuyCommodityResult, DataPacket::emNormal, S2CPacketBuyCommodityResult)
_SERIALIZE_MEMBER(i32, code)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ShowBuyRespawn, DataPacket::emNormal, S2CPacketShowBuyRespawn)
_SERIALIZE_MEMBER(i32, times)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::BuyRespawnResult, DataPacket::emNormal, S2CPacketBuyRespawnResult)
_SERIALIZE_MEMBER(i32, code)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::ShowGoNpcMerchant, DataPacket::emNormal, S2CPacketShowGoNpcMerchant)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, x)
_SERIALIZE_MEMBER(float, y)
_SERIALIZE_MEMBER(float, z)
_SERIALIZE_MEMBER(float, yaw)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::UpdateBackpack, DataPacket::emNormal, S2CPacketUpdateBackpack)
_SERIALIZE_MEMBER(i32, capacity)
_SERIALIZE_MEMBER(i32, maxCapacity)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::RankData, DataPacket::emNormal, S2CPacketRankData)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

//S2CPacketActivateTrigger
_BEGIN_PACKET_S2C(S2CPacketType::ActivateTrigger, DataPacket::emNormal, S2CPacketActivateTrigger)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, blockId)
_SERIALIZE_MEMBER(i32, triggerType)
_SERIALIZE_MEMBER(ui32, triggerIndex)
_END_PACKET

//S2CPacketUpdateRankData
_BEGIN_PACKET_S2C(S2CPacketType::UpdateRankNpc, DataPacket::emNormal, S2CPacketUpdateRankNpc)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, rankInfo)
_END_PACKET

//S2CPacketTakeAircraft
_BEGIN_PACKET_S2C(S2CPacketType::TakeAircraft, DataPacket::emNormal, S2CPacketTakeAircraft)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, passengerId)
_SERIALIZE_MEMBER(bool8, isOn)
_END_PACKET

//S2CPacketAircraftStartFly
_BEGIN_PACKET_S2C(S2CPacketType::AircraftStartFly, DataPacket::emNormal, S2CPacketAircraftStartFly)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, speed)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, startPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, endPos)
_END_PACKET

//S2CPacketSyncAircraftState
_BEGIN_PACKET_S2C(S2CPacketType::SyncAircraftState, DataPacket::emNormal, S2CPacketSyncAircraftState)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, flyingTick)
_SERIALIZE_MEMBER(bool8, isFlying)
_SERIALIZE_MEMBER(float, speed)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, startPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, endPos)
_SERIALIZE_MEMBER(vector<i32>::type, m_passengers)
_END_PACKET

//S2CPacketFriendOperation
_BEGIN_PACKET_S2C(S2CPacketType::FriendOperation, DataPacket::emNormal, S2CPacketFriendOperation)
_SERIALIZE_MEMBER(i32, operationType)
_SERIALIZE_MEMBER(ui64, userId)
_END_PACKET

//S2CPacketChangeAircraftUI
_BEGIN_PACKET_S2C(S2CPacketType::ChangeAircraftUI, DataPacket::emNormal, S2CPacketChangeAircraftUI)
_SERIALIZE_MEMBER(bool8, isShowUI)
_END_PACKET

//S2CPacketAirdrop
_BEGIN_PACKET_S2C(S2CPacketType::AirDorp, DataPacket::emNormal, S2CPacketAirdrop)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, m_airDropPos)
_END_PACKET

//S2CPacketPoisonCircleRange
_BEGIN_PACKET_S2C(S2CPacketType::PoisonCircleRange, DataPacket::emNormal, S2CPacketPoisonCircleRange)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, minSafeRange)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, maxSafeRange)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, minPoisonRange)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, maxPoisonRange)
_SERIALIZE_MEMBER(float, speed)
_END_PACKET

//S2CPacketMemberLeftAndKill
_BEGIN_PACKET_S2C(S2CPacketType::MemberLeftAndKill, DataPacket::emNormal, S2CPacketMemberLeftAndKill)
_SERIALIZE_MEMBER(i32, m_left)
_SERIALIZE_MEMBER(i32, m_kill)
_END_PACKET

//S2CPacketSyncBlockDestroyeFailure
_BEGIN_PACKET_S2C(S2CPacketType::SyncBlockDestroyeFailure, DataPacket::emNormal, S2CPacketSyncBlockDestroyeFailure)
_SERIALIZE_MEMBER(Bool, destroyeStatus)
_SERIALIZE_MEMBER(i32, destroyeFailureType)
_END_PACKET


//S2CPacketSkillType
_BEGIN_PACKET_S2C(S2CPacketType::SkillType, DataPacket::emNormal, S2CPacketSkillType)
_SERIALIZE_MEMBER(i32, skillType)
_SERIALIZE_MEMBER(float, duration)
_SERIALIZE_MEMBER(float, coldDownTime)
_END_PACKET


//S2CSyncChangePlayerActorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncChangePlayerActorInfo, DataPacket::emNormal, S2CSyncChangePlayerActorInfo)
_SERIALIZE_MEMBER(int, changeActorCount)
_SERIALIZE_MEMBER(int, needMoneyCount)
_SERIALIZE_MEMBER(String, actorName)
_END_PACKET

//S2CSyncNpcActorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncNpcActorInfo, DataPacket::emNormal, S2CSyncNpcActorInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isCanObstruct)
_SERIALIZE_MEMBER(bool, isCanCollided)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, headName)
_SERIALIZE_MEMBER(String, skillName)
_SERIALIZE_MEMBER(String, haloEffectName)
_SERIALIZE_MEMBER(String, content)
_END_PACKET

//S2CPacketHideAndSeekBtnStatus
_BEGIN_PACKET_S2C(S2CPacketType::SyncHideAndSeekBtnStatus, DataPacket::emNormal, S2CPacketHideAndSeekBtnStatus)
_SERIALIZE_MEMBER(bool, isShowChangeActorBtn)
_SERIALIZE_MEMBER(bool, isShowChangeCameraBtn)
_SERIALIZE_MEMBER(bool, isShowOtherBtn)
_END_PACKET

//S2CPacketEntitySkillEffect
_BEGIN_PACKET_S2C(S2CPacketType::EntitySkillEffect, DataPacket::emNormal, S2CPacketEntitySkillEffect)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(float, duration)
_SERIALIZE_MEMBER(i32, width)
_SERIALIZE_MEMBER(i32, height)
_SERIALIZE_MEMBER(int, density)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, color)
_END_PACKET

//S2CPacketPickUpItemOrder
_BEGIN_PACKET_S2C(S2CPacketType::PickUpItemOrder, DataPacket::emNormal, S2CPacketPickUpItemOrder)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, pickUpPrice)
_SERIALIZE_MEMBER(i32, moneyType)
_SERIALIZE_MEMBER(i32, itemEntityId)
_END_PACKET

//S2CPacketRequestResult
_BEGIN_PACKET_S2C(S2CPacketType::RequestResult, DataPacket::emNormal, S2CPacketRequestResult)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, resultCode)
_SERIALIZE_MEMBER(String, resultMessage)
_END_PACKET

//S2CPacketSyncGameTimeShowUi
_BEGIN_PACKET_S2C(S2CPacketType::SyncGameTimeShowUi, DataPacket::emNormal, S2CPacketSyncGameTimeShowUi)
_SERIALIZE_MEMBER(int, time)
_SERIALIZE_MEMBER(bool, isShowUI)
_END_PACKET

//S2CPacketSyncCreateArrowTipMark
_BEGIN_PACKET_S2C(S2CPacketType::SyncCreateArrowTipMark, DataPacket::emNormal, S2CPacketSyncCreateArrowTipMark)
_SERIALIZE_MEMBER(int, destoryTime)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, sourcePostion)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, targetPostion)
_SERIALIZE_MEMBER(float, distances)
_END_PACKET

//S2CPacketSyncCameraLock
_BEGIN_PACKET_S2C(S2CPacketType::SyncCameraLock, DataPacket::emNormal, S2CPacketSyncCameraLock)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isCameraLock)
_END_PACKET

//S2CPacketDeathUnequipArmor
_BEGIN_PACKET_S2C(S2CPacketType::DeathUnequipArmor, DataPacket::emNormal, S2CPacketDeathUnequipArmor)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//S2CPacketSyncChangeEntityPerspece
_BEGIN_PACKET_S2C(S2CPacketType::SyncChangeEntityPerspece, DataPacket::emNormal, S2CPacketSyncChangeEntityPerspece)
_SERIALIZE_MEMBER(int, View)
_END_PACKET

//S2CPacketPlayerInvisible
_BEGIN_PACKET_S2C(S2CPacketType::PlayerInvisible, DataPacket::emNormal, S2CPacketPlayerInvisible)
_SERIALIZE_MEMBER(bool, isInvisible)
_SERIALIZE_MEMBER(i32, entityId)
_END_PACKET

//S2CPacketChangeHeart
_BEGIN_PACKET_S2C(S2CPacketType::ChangeHeart, DataPacket::emNormal, S2CPacketChangeHeart)
_SERIALIZE_MEMBER(i32, hp)
_SERIALIZE_MEMBER(i32, maxHp)
_END_PACKET

//S2CPacketSyncSessionNpc
_BEGIN_PACKET_S2C(S2CPacketType::SyncSessionNpc, DataPacket::emNormal, S2CPacketSyncSessionNpc)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, sessionType)
_SERIALIZE_MEMBER(i32, timeLeft)
_SERIALIZE_MEMBER(bool, isPerson)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorAction)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(String, sessionContent)
_SERIALIZE_MEMBER(String, effectName)
_SERIALIZE_MEMBER(bool, isCanCollided)
_END_PACKET

//S2CPacketUpdateManor
_BEGIN_PACKET_S2C(S2CPacketType::UpdateManor, DataPacket::emNormal, S2CPacketUpdateManor)
_SERIALIZE_MEMBER(BLOCKMAN::ManorInfo, m_manorInfo)
_SERIALIZE_MEMBER(BLOCKMAN::ManorInfo, m_nextManorInfo)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorHouse>::type, m_manorHouses)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorFurniture>::type, m_manorFurnitures)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorMessage>::type, m_manorMessages)
_END_PACKET

//S2CPacketUpdateRealTimeRankInfo
_BEGIN_PACKET_S2C(S2CPacketType::UpdateRealTimeRankInfo, DataPacket::emNormal, S2CPacketUpdateRealTimeRankInfo)
_SERIALIZE_MEMBER(String, result)
_END_PACKET

//S2CPacketUpdateGameMonsterInfo
_BEGIN_PACKET_S2C(S2CPacketType::UpdateGameMonsterInfo, DataPacket::emNormal, S2CPacketUpdateGameMonsterInfo)
_SERIALIZE_MEMBER(i32, currCheckPointNum)
_SERIALIZE_MEMBER(i32, surplusMonsterNum)
_SERIALIZE_MEMBER(i32, surplusCheckPointNum)
_END_PACKET

//S2CPacketUpdateBossBloodStrip
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBossBloodStrip, DataPacket::emNormal, S2CPacketUpdateBossBloodStrip)
_SERIALIZE_MEMBER(i32, currBloodStrip)
_SERIALIZE_MEMBER(i32, maxBloodStrip)
_END_PACKET

// S2CPacketSwitchablePropsData
_BEGIN_PACKET_S2C(S2CPacketType::SwitchablePropsData, DataPacket::emNormal, S2CPacketSwitchablePropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketUpgradePropsData
_BEGIN_PACKET_S2C(S2CPacketType::UpgradePropsData, DataPacket::emNormal, S2CPacketUpgradePropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketPersonalShopTip
_BEGIN_PACKET_S2C(S2CPacketType::PersonalShopTip, DataPacket::emNormal, S2CPacketPersonalShopTip)
_SERIALIZE_MEMBER(String, tip)
_END_PACKET

// S2CPacketCreateHouseFromSchematic
_BEGIN_PACKET_S2C(S2CPacketType::CreateHouseFromSchematic, DataPacket::emNormal, S2CPacketCreateHouseFromSchematic)
_SERIALIZE_MEMBER(String, fileName)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3i, start_pos)
_SERIALIZE_MEMBER(bool, xImage)
_SERIALIZE_MEMBER(bool, zImage)
_SERIALIZE_MEMBER(bool, createOrDestroy)
_END_PACKET


// S2CPacketFillAreaByBlockIdAndMate
_BEGIN_PACKET_S2C(S2CPacketType::FillAreaByBlockIdAndMate, DataPacket::emNormal, S2CPacketFillAreaByBlockIdAndMate)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3i, start_pos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3i, end_pos)
_SERIALIZE_MEMBER(i32, block_id)
_SERIALIZE_MEMBER(i32, mate)
_END_PACKET


// S2CPacketShowPlayerOperation
_BEGIN_PACKET_S2C(S2CPacketType::ShowPlayerOperation, DataPacket::emNormal, S2CPacketShowPlayerOperation)
_SERIALIZE_MEMBER(ui64, m_targetUserId)
_SERIALIZE_MEMBER(String, m_targetUserName)
_END_PACKET


//S2CPacketSyncEntityCreature
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityCreature, DataPacket::emNormal, S2CPacketSyncEntityCreature)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(int, creatureEntityType)
_END_PACKET

//S2CPacketSyncCreatureEntityAction
_BEGIN_PACKET_S2C(S2CPacketType::SyncCreatureEntityAction, DataPacket::emNormal, S2CPacketSyncCreatureEntityAction)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(int, actionStatus)
_SERIALIZE_MEMBER(int, baseAction)
_SERIALIZE_MEMBER(int, preBaseAction)
_END_PACKET

//S2CPacketPlayerSpeedLevel
_BEGIN_PACKET_S2C(S2CPacketType::PlayerSpeedLevel, DataPacket::emNormal, S2CPacketPlayerSpeedLevel)
_SERIALIZE_MEMBER(i32, speedLevel)
_END_PACKET

//S2CPacketPlayerExpInfo
_BEGIN_PACKET_S2C(S2CPacketType::PlayerExpInfo, DataPacket::emNormal, S2CPacketPlayerExpInfo)
_SERIALIZE_MEMBER(i32, level)
_SERIALIZE_MEMBER(float, exp)
_SERIALIZE_MEMBER(i32, maxExp)
_END_PACKET

//S2CPacketSpwanCreatureBullet
_BEGIN_PACKET_S2C(S2CPacketType::SpawnCreatureBullet, DataPacket::emNormal, S2CPacketSpawnCreatureBullet)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(int, type)
_SERIALIZE_MEMBER(int, throwerId)
_SERIALIZE_MEMBER(int, targeterId)
_END_PACKET

//S2CPacketShowUpgradeResourceUI
_BEGIN_PACKET_S2C(S2CPacketType::ShowUpgradeResourceUI, DataPacket::emNormal, S2CPacketShowUpgradeResourceUI)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, itemNum)
_SERIALIZE_MEMBER(i32, resourceId)
_SERIALIZE_MEMBER(String, tipString)
_END_PACKET

// S2CPacketManorBtnVisible
_BEGIN_PACKET_S2C(S2CPacketType::ManorBtnVisible, DataPacket::emNormal, S2CPacketManorBtnVisible)
_SERIALIZE_MEMBER(bool, visible)
_END_PACKET

// S2CPacketCallOnManorResetClient
_BEGIN_PACKET_S2C(S2CPacketType::CallOnManorResetClient, DataPacket::emNormal, S2CPacketCallOnManorResetClient)
_SERIALIZE_MEMBER(ui64, m_targetUserId)
_END_PACKET

// S2CPacketCallOnManorResetClient
_BEGIN_PACKET_S2C(S2CPacketType::UpdateManorOwner, DataPacket::emNormal, S2CPacketUpdateManorOwner)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ManorOwner>::type, m_owners)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerMovementInput, DataPacket::emNormal, S2CPacketSyncPlayerMovementInput)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(float, forward)
_SERIALIZE_MEMBER(float, strafe)
_END_PACKET

//S2CPacketUpdateBossBloodStrip
_BEGIN_PACKET_S2C(S2CPacketType::UpdateBasementLife, DataPacket::emNormal, S2CPacketUpdateBasementLife)
_SERIALIZE_MEMBER(float, curLife)
_SERIALIZE_MEMBER(float, maxLife)
_END_PACKET

//S2CPacketShowBuildWarBlockBtn
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarBlockBtn, DataPacket::emNormal, S2CPacketShowBuildWarBlockBtn)
_SERIALIZE_MEMBER(bool, isNeedOpen)
_END_PACKET

//S2CPacketShowBuildWarPlayNum
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarPlayNum, DataPacket::emNormal, S2CPacketShowBuildWarPlayNum)
_SERIALIZE_MEMBER(ui8, cur_num)
_SERIALIZE_MEMBER(ui8, max_num)
_END_PACKET

//S2CPacketShowBuildWarLeftTime
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarLeftNum, DataPacket::emNormal, S2CPacketShowBuildWarLeftTime)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(ui16, left_time)
_SERIALIZE_MEMBER(String, theme)
_END_PACKET

//S2CPacketOpenShopByEntityId
_BEGIN_PACKET_S2C(S2CPacketType::OpenShopByEntityId, DataPacket::emNormal, S2CPacketOpenShopByEntityId)
_SERIALIZE_MEMBER(int, entityId)
_END_PACKET

//S2CPacketShowGrade
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarGrade, DataPacket::emNormal, S2CPacketShowGrade)
_SERIALIZE_MEMBER(bool, openOrClose)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, msg)
_SERIALIZE_MEMBER(ui8, left_time)
_END_PACKET

//S2CPacketShowGuessResult
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarGuessResult, DataPacket::emNormal, S2CPacketShowGuessResult)
_SERIALIZE_MEMBER(bool, right)
_SERIALIZE_MEMBER(int, rank)
_END_PACKET

//S2CPacketShowGuessUi
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuildWarGuessUi, DataPacket::emNormal, S2CPacketShowGuessUi)
_SERIALIZE_MEMBER(String, info)
_SERIALIZE_MEMBER(int, guess_room_id)
_END_PACKET

// S2CPacketGameSettlementExtra
_BEGIN_PACKET_S2C(S2CPacketType::GameSettlementExtra, DataPacket::emNormal, S2CPacketGameSettlementExtra)
_SERIALIZE_MEMBER(bool, guess_right)
_SERIALIZE_MEMBER(String, guess_name)
_SERIALIZE_MEMBER(int, guess_reward)
_END_PACKET

//S2CPacketUnlockedCommodity
_BEGIN_PACKET_S2C(S2CPacketType::UnlockedCommodity, DataPacket::emNormal, S2CPacketUnlockedCommodity)
_SERIALIZE_MEMBER(i32, merchantId)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_END_PACKET

//S2CPacketInitUnlockedCommodity
_BEGIN_PACKET_S2C(S2CPacketType::InitUnlockedCommodity, DataPacket::emNormal, S2CPacketInitUnlockedCommodity)
_SERIALIZE_MEMBER(i32, itemId)
_SERIALIZE_MEMBER(i32, meta)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerRotation, DataPacket::emNormal, S2CPacketSyncPlayerRotation)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(float, rotationYaw)
_SERIALIZE_MEMBER(float, rotationPitch)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerJumpChanged, DataPacket::emNormal, S2CPacketSyncPlayerJumpChanged)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isJump)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerDownChanged, DataPacket::emNormal, S2CPacketSyncPlayerDownChanged)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isDown)
_END_PACKET

_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerSetFlyingState, DataPacket::emNormal, S2CPacketSyncPlayerSetFlyingState)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isFlying)
_END_PACKET

//S2CPacketEntityTeleport
_BEGIN_PACKET_S2C(S2CPacketType::EntityPlayerTeleport, DataPacket::emNormal, S2CPacketEntityPlayerTeleport)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity)
	{
		m_entityId = pEntity->entityId;
		m_x = pEntity->position.x;
		m_y = pEntity->position.y;
		m_z = pEntity->position.z;
	}

	S2CPacket(int eid, real32 x, real32 y, real32 z)
		: m_entityId(eid)
		, m_x(x)
		, m_y(y)
		, m_z(z)
	{}
	_SERIALIZE_MEMBER(i32, m_entityId)
		_SERIALIZE_MEMBER(real32, m_x)
		_SERIALIZE_MEMBER(real32, m_y)
		_SERIALIZE_MEMBER(real32, m_z)
		_END_PACKET

// S2CPacketUpdateChestInventory
_BEGIN_PACKET_S2C(S2CPacketType::UpdateChestInventory, DataPacket::emNormal, S2CPacketUpdateChestInventory)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i16, blockPosX)
_SERIALIZE_MEMBER(i16, blockPosY)
_SERIALIZE_MEMBER(i16, blockPosZ)
_SERIALIZE_MEMBER(i32, face)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(vector<NETWORK_DEFINE::ItemStackInfo>::type, chestInventory)
_END_PACKET

//S2CPacketUpdateMerchantInfo
_BEGIN_PACKET_S2C(S2CPacketType::UpdateMerchantInfo, DataPacket::emNormal, S2CPacketUpdateMerchantInfo)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, name)
_END_PACKET

//S2CPacketUpdateMerchantCommodities
_BEGIN_PACKET_S2C(S2CPacketType::UpdateMerchantCommodities, DataPacket::emNormal, S2CPacketUpdateMerchantCommodities)
_SERIALIZE_MEMBER(i32, id)
_SERIALIZE_MEMBER(vector<BLOCKMAN::CommodityGroup>::type, commodities)
_END_PACKET

//S2CPacketCustomTipMsg
_BEGIN_PACKET_S2C(S2CPacketType::CustomTipMsg, DataPacket::emNormal, S2CPacketCustomTipMsg)
_SERIALIZE_MEMBER(String, messageLang)
_SERIALIZE_MEMBER(String, extra)
_END_PACKET


//S2CPacketEntityPlayerOccupation
_BEGIN_PACKET_S2C(S2CPacketType::EntityPlayerOccupation, DataPacket::emNormal, S2CPacketEntityPlayerOccupation)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(int , occupation)
_END_PACKET

//S2CPacketSpwanSkillItemThrowable
_BEGIN_PACKET_S2C(S2CPacketType::SpwanSkillItemThrowable, DataPacket::emNormal, S2CPacketSpwanSkillItemThrowable)
_SERIALIZE_MEMBER(int, throwerId)
_SERIALIZE_MEMBER(int, skillId)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, motion)
_END_PACKET

//S2CPacketTeamResourcesUpdate
_BEGIN_PACKET_S2C(S2CPacketType::TeamResourcesUpdate, DataPacket::emNormal, S2CPacketTeamResourcesUpdate)
_SERIALIZE_MEMBER(String, m_data)
_END_PACKET

//S2CPacketSyncShowMaskTime
_BEGIN_PACKET_S2C(S2CPacketType::SyncShowMaskTime, DataPacket::emNormal, S2CPacketSyncShowMaskTime)
_SERIALIZE_MEMBER(int, MilliSecond)
_END_PACKET

//S2CPacketCloseBGM
_BEGIN_PACKET_S2C(S2CPacketType::CloseBGM, DataPacket::emNormal, S2CPacketCloseBGM)
_END_PACKET

//S2CPacketSetArmStack
_BEGIN_PACKET_S2C(S2CPacketType::SetArmItem, DataPacket::emNormal, S2CPacketSetArmItem)
_SERIALIZE_MEMBER(int, itemId)
_END_PACKET

//S2CPacketActorNpcContent
_BEGIN_PACKET_S2C(S2CPacketType::ActorNpcContent, DataPacket::emNormal, S2CPacketActorNpcContent)
_SERIALIZE_MEMBER(int, entityId)
_SERIALIZE_MEMBER(String, content)
_END_PACKET

//S2CPacketBuildProgress
_BEGIN_PACKET_S2C(S2CPacketType::BuildProgress, DataPacket::emNormal, S2CPacketBuildProgress)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(int, curProgress)
_SERIALIZE_MEMBER(int, maxProgress)
_SERIALIZE_MEMBER(String, iconPath)
_END_PACKET

// S2CPacketEnchantmentPropsData
_BEGIN_PACKET_S2C(S2CPacketType::EnchantmentPropsData, DataPacket::emNormal, S2CPacketEnchantmentPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketPersonalSuperPropsData
_BEGIN_PACKET_S2C(S2CPacketType::PersonalSuperPropsData, DataPacket::emNormal, S2CPacketPersonalSuperPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketEnableMovement
_BEGIN_PACKET_S2C(S2CPacketType::EnableMovement, DataPacket::emNormal, S2CPacketEnableMovement)
_SERIALIZE_MEMBER(int, entityId)
_END_PACKET

//S2CPacketConsumeCoinTip
_BEGIN_PACKET_S2C(S2CPacketType::ConsumeCoinTip, DataPacket::emNormal, S2CPacketConsumeCoinTip)
_SERIALIZE_MEMBER(String, message)
_SERIALIZE_MEMBER(int, coinId)
_SERIALIZE_MEMBER(int, price)
_SERIALIZE_MEMBER(String, extra)
_END_PACKET

// S2CPacketSuperPropsData
_BEGIN_PACKET_S2C(S2CPacketType::SuperPropsData, DataPacket::emNormal, S2CPacketSuperPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketSuperShopTip
_BEGIN_PACKET_S2C(S2CPacketType::SuperShopTip, DataPacket::emNormal, S2CPacketSuperShopTip)
_SERIALIZE_MEMBER(String, tip)
_END_PACKET

//S2CPacketChangeDefense
_BEGIN_PACKET_S2C(S2CPacketType::ChangeDefense, DataPacket::emNormal, S2CPacketChangeDefense)
_SERIALIZE_MEMBER(i32, defense)
_SERIALIZE_MEMBER(i32, maxDefense)
_END_PACKET

//S2CPacketChangeAttack
_BEGIN_PACKET_S2C(S2CPacketType::ChangeAttack, DataPacket::emNormal, S2CPacketChangeAttack)
_SERIALIZE_MEMBER(i32, attack)
_SERIALIZE_MEMBER(i32, maxAttack)
_END_PACKET

//S2CPacketAddSimpleEffect
_BEGIN_PACKET_S2C(S2CPacketType::AddSimpleEffect, DataPacket::emNormal, S2CPacketAddSimpleEffect)
_SERIALIZE_MEMBER(String, effectName)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(int, duration)
_SERIALIZE_MEMBER(float, scale)
_END_PACKET

//S2CPacketEnterOtherGame
_BEGIN_PACKET_S2C(S2CPacketType::EnterOtherGame, DataPacket::emNormal, S2CPacketEnterOtherGame)
_SERIALIZE_MEMBER(String, gameType)
_SERIALIZE_MEMBER(ui64, targetId)
_SERIALIZE_MEMBER(String, mapId)
_END_PACKET

//S2CPacketKillMsg
_BEGIN_PACKET_S2C(S2CPacketType::KillMsg, DataPacket::emNormal, S2CPacketKillMsg)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketShowPersonalShop
_BEGIN_PACKET_S2C(S2CPacketType::ShowPersonalShop, DataPacket::emNormal, S2CPacketShowPersonalShop)
_END_PACKET

//S2CPacketPersonalShopArea
_BEGIN_PACKET_S2C(S2CPacketType::PersonalShopArea, DataPacket::emNormal, S2CPacketPersonalShopArea)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, startPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, endPos)
_END_PACKET

//S2CPacketAddGunBulletNum
_BEGIN_PACKET_S2C(S2CPacketType::AddGunBulletNum, DataPacket::emNormal, S2CPacketAddGunBulletNum)
_SERIALIZE_MEMBER(int, gunId)
_SERIALIZE_MEMBER(int, bulletNum)
_END_PACKET

//S2CPacketSubGunRecoil
_BEGIN_PACKET_S2C(S2CPacketType::SubGunRecoil, DataPacket::emNormal, S2CPacketSubGunRecoil)
_SERIALIZE_MEMBER(int, gunId)
_SERIALIZE_MEMBER(float, recoil)
_END_PACKET

// S2CPacketCustomPropsData
_BEGIN_PACKET_S2C(S2CPacketType::CustomPropsData, DataPacket::emNormal, S2CPacketCustomPropsData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

// S2CPacketUpdateCustomProps
_BEGIN_PACKET_S2C(S2CPacketType::UpdateCustomProps, DataPacket::emNormal, S2CPacketUpdateCustomProps)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketSyncEntityBlockmanInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBlockman, DataPacket::emNormal, S2CPacketSyncEntityBlockmanInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, blockmanType)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(float, maxSpeed)
_SERIALIZE_MEMBER(float, curSpeed)
_SERIALIZE_MEMBER(float, addSpeed)
_SERIALIZE_MEMBER(bool, isRecordRoute)
_SERIALIZE_MEMBER(int, longHitTimes)
_END_PACKET

//S2CPacketBlockmanMoveAttr
_BEGIN_PACKET_S2C(S2CPacketType::BlockmanMoveAttr, DataPacket::emNormal, S2CPacketBlockmanMoveAttr)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, pushX)
_SERIALIZE_MEMBER(float, pushZ)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, position)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, motion)
_END_PACKET

//S2CPacketCarDirection
_BEGIN_PACKET_S2C(S2CPacketType::CarDirection, DataPacket::emNormal, S2CPacketCarDirection)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, direction)
_END_PACKET

//S2CPacketCarProgress
_BEGIN_PACKET_S2C(S2CPacketType::CarProgress, DataPacket::emNormal, S2CPacketCarProgress)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(float, progress)
_END_PACKET

//S2CPacketEntityFrozenTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityFrozenTime, DataPacket::emNormal, S2CPacketEntityFrozenTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketEntityTeleportWithMotion
_BEGIN_PACKET_S2C(S2CPacketType::EntityPlayerTeleportWithMotion, DataPacket::emNormal, S2CPacketEntityPlayerTeleportWithMotion)
public:
	S2CPacket(BLOCKMAN::Entity* pEntity)
	{
		m_entityId = pEntity->entityId;
		m_x = pEntity->position.x;
		m_y = pEntity->position.y;
		m_z = pEntity->position.z;
		m_mx = pEntity->motion.x;
		m_my = pEntity->motion.y;
		m_mz = pEntity->motion.z;
		m_isOnGround = pEntity->onGround;
	}
_SERIALIZE_MEMBER(i32, m_entityId)
_SERIALIZE_MEMBER(real32, m_x)
_SERIALIZE_MEMBER(real32, m_y)
_SERIALIZE_MEMBER(real32, m_z)
_SERIALIZE_MEMBER(real32, m_mx)
_SERIALIZE_MEMBER(real32, m_my)
_SERIALIZE_MEMBER(real32, m_mz)
_SERIALIZE_MEMBER(bool, m_isOnGround)
_END_PACKET

//S2CSyncCannonActorInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncCannonActorInfo, DataPacket::emNormal, S2CSyncCannonActorInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isCanObstruct)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, headName)
_SERIALIZE_MEMBER(String, skillName)
_SERIALIZE_MEMBER(String, haloEffectName)
_SERIALIZE_MEMBER(String, content)
_END_PACKET

//S2CPacketSpawnCannon
_BEGIN_PACKET_S2C(S2CPacketType::SpawnCannon, DataPacket::emNormal, S2CPacketSpawnCannon)
public:
	S2CPacket(BLOCKMAN::EntityActorCannon* pEntity)
	{
		m_entityID = pEntity->entityId;
		m_pos = pEntity->position;
		m_fallOnPos = pEntity->m_fallOnPos;
		m_yaw = pEntity->rotationYaw;
		m_pitch = pEntity->rotationPitch;
	}
_SERIALIZE_MEMBER(i32, m_entityID)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, m_pos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, m_fallOnPos)
_SERIALIZE_MEMBER(real32, m_pitch)
_SERIALIZE_MEMBER(real32, m_yaw)
_END_PACKET

// S2CPacketSyncCannonFire
_BEGIN_PACKET_S2C(S2CPacketType::SyncCannonFire, DataPacket::emNormal, S2CPacketSyncCannonFire)
_SERIALIZE_MEMBER(i32, cannonEntityId)
_END_PACKET

//S2CPacketSyncRanchInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchInfo, DataPacket::emNormal, S2CPacketSyncRanchInfo)
_SERIALIZE_MEMBER(BLOCKMAN::RanchInfo, m_info)
_END_PACKET

//S2CPacketSyncRanchHouse
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchHouse, DataPacket::emNormal, S2CPacketSyncRanchHouse)
_SERIALIZE_MEMBER(BLOCKMAN::RanchHouse, m_house)
_END_PACKET

//S2CPacketSyncRanchStorage
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchStorage, DataPacket::emNormal, S2CPacketSyncRanchStorage)
_SERIALIZE_MEMBER(BLOCKMAN::RanchStorage, m_storage)
_END_PACKET

//S2CPacketSyncRanchRank
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchRank, DataPacket::emNormal, S2CPacketSyncRanchRank)
_SERIALIZE_MEMBER(i32, m_rankType)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchRank>::type, m_ranks)
_END_PACKET

//S2CPacketSyncRanchBuild
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchBuild, DataPacket::emNormal, S2CPacketSyncRanchBuild)
_SERIALIZE_MEMBER(i32, m_buildType)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchBuild>::type, m_builds)
_END_PACKET

//S2CPacketSyncEntityBuildNpc
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBuildNpc, DataPacket::emNormal, S2CPacketSyncEntityBuildNpc)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, m_actorId)
_SERIALIZE_MEMBER(ui64, m_userId)

_SERIALIZE_MEMBER(i32, m_productCapacity)
_SERIALIZE_MEMBER(i32, m_maxQueueNum)
_SERIALIZE_MEMBER(i32, m_queueUnlockPrice)
_SERIALIZE_MEMBER(i32, m_queueUnlockCurrencyType)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ProductRecipe>::type, m_recipes)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, m_products)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ProductQueue>::type, m_unlockQueues)
_END_PACKET

//S2CPacketSyncEntityLandNpc
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityLandNpc, DataPacket::emNormal, S2CPacketSyncEntityLandNpc)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(ui64, userId)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(i32, status)
_SERIALIZE_MEMBER(i32, timeLeft)
_SERIALIZE_MEMBER(i32, landCode)
_SERIALIZE_MEMBER(i32, price)
_SERIALIZE_MEMBER(i32, totalTime)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, recipe)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, reward)
_END_PACKET


//S2CPacketRanchGain
_BEGIN_PACKET_S2C(S2CPacketType::RanchGain, DataPacket::emNormal, S2CPacketRanchGain)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchCommon>::type, items)
_END_PACKET

//S2CPacketSyncRanchOrder
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchOrder, DataPacket::emNormal, S2CPacketSyncRanchOrder)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchOrder>::type, m_orders)
_END_PACKET

//S2CPacketSyncRanchAchievement
_BEGIN_PACKET_S2C(S2CPacketType::SyncRanchAchievement, DataPacket::emNormal, S2CPacketSyncRanchAchievement)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchAchievement>::type, m_achievements)
_END_PACKET

//S2CPacketSyncRanchTimePrice
_BEGIN_PACKET_S2C(S2CPacketType::RanchTimePrice, DataPacket::emNormal, S2CPacketSyncRanchTimePrice)
_SERIALIZE_MEMBER(vector<BLOCKMAN::RanchTimePrice>::type, m_timePrices)
_END_PACKET

//S2CPacketSyncRanchShortcutFreeTimes
_BEGIN_PACKET_S2C(S2CPacketType::RanchShortcutFreeTimes, DataPacket::emNormal, S2CPacketSyncRanchShortcutFreeTimes)
_SERIALIZE_MEMBER(vector<vector<i32>::type>::type , m_shortcutFreeTimes)
_END_PACKET

//S2CPacketSyncPlayerSpecialJump
_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerSpecialJump, DataPacket::emNormal, S2CPacketSyncPlayerSpecialJump)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(float, yFactor)
_SERIALIZE_MEMBER(float, xFactor)
_SERIALIZE_MEMBER(float, zFactor)
_END_PACKET

//S2CPacketShowRanchExTip
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExTip, DataPacket::emNormal, S2CPacketShowRanchExTip)
_SERIALIZE_MEMBER(ui16, type)
_SERIALIZE_MEMBER(ui16, num)
_END_PACKET

//S2CPacketShowRanchExTask
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExTask, DataPacket::emNormal, S2CPacketShowRanchExTask)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, task)
_END_PACKET

//S2CPacketBroadcastMessage
_BEGIN_PACKET_S2C(S2CPacketType::BroadcastMessage, DataPacket::emNormal, S2CPacketBroadcastMessage)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(String, content)
_END_PACKET

//S2CPacketGotoOtherGame
_BEGIN_PACKET_S2C(S2CPacketType::GotoOtherGame, DataPacket::emNormal, S2CPacketGotoOtherGame)
_SERIALIZE_MEMBER(ui64, targetUserId)
_SERIALIZE_MEMBER(String, gameType)
_SERIALIZE_MEMBER(String, mapId)
_END_PACKET

//S2CPacketRanchUnlockItem
_BEGIN_PACKET_S2C(S2CPacketType::RanchUnlockItem, DataPacket::emNormal, S2CPacketRanchUnlockItem)
_SERIALIZE_MEMBER(vector<i32>::type, items)
_END_PACKET

//S2CPacketShowRanchExCurrentItemInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExCurrentItemInfo, DataPacket::emNormal, S2CPacketShowRanchExCurrentItemInfo)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, itemInfo)
_SERIALIZE_MEMBER(i32, type)
_SERIALIZE_MEMBER(i32, num)
_END_PACKET

//S2CPacketShowRanchExItem
_BEGIN_PACKET_S2C(S2CPacketType::ShowRanchExItem, DataPacket::emNormal, S2CPacketShowRanchExItem)
_SERIALIZE_MEMBER(bool, show)
_SERIALIZE_MEMBER(String, item)
_END_PACKET

//S2CPacketCropInfo
_BEGIN_PACKET_S2C(S2CPacketType::CropInfo, DataPacket::emNormal, S2CPacketCropInfo)
_SERIALIZE_MEMBER(Vector3i, blockPos)
_SERIALIZE_MEMBER(i32, blockId)
_SERIALIZE_MEMBER(i32, stage)
_SERIALIZE_MEMBER(i32, curStageTime)
_SERIALIZE_MEMBER(i32, residueHarvestNum)
_END_PACKET

//S2CPacketCropInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowBuyKeepItemTip, DataPacket::emNormal, S2CPacketShowBuyKeepItemTip)
_SERIALIZE_MEMBER(int, coinType)
_SERIALIZE_MEMBER(int, coin)
_SERIALIZE_MEMBER(int, tipTime)
_END_PACKET

//S2CPacketSendOpenEnchantment
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenEnchantment, DataPacket::emNormal, S2CPacketSendOpenEnchantment)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketAppExpResult
_BEGIN_PACKET_S2C(S2CPacketType::AppExpResult, DataPacket::emNormal, S2CPacketAppExpResult)
_SERIALIZE_MEMBER(i32, curLv)
_SERIALIZE_MEMBER(i32, toLv)
_SERIALIZE_MEMBER(i32, addExp)
_SERIALIZE_MEMBER(i32, curExp)
_SERIALIZE_MEMBER(i32, toExp)
_SERIALIZE_MEMBER(i32, upExp)
_SERIALIZE_MEMBER(i32, status)
_END_PACKET

//S2CPacketOccupationUnlock
_BEGIN_PACKET_S2C(S2CPacketType::OccupationUnlock, DataPacket::emNormal, S2CPacketOccupationUnlock)
_SERIALIZE_MEMBER(String, occupationInfo)
_END_PACKET

//S2CPacketBuyFlying
_BEGIN_PACKET_S2C(S2CPacketType::BuyFlying, DataPacket::emNormal, S2CPacketBuyFlying)
_END_PACKET

//S2CPacketPlayCloseup
_BEGIN_PACKET_S2C(S2CPacketType::PlayCloseup, DataPacket::emNormal, S2CPacketPlayCloseup)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, playerPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, closeupPos)
_SERIALIZE_MEMBER(float, farDistance)
_SERIALIZE_MEMBER(float, nearDistance)
_SERIALIZE_MEMBER(float, velocity)
_SERIALIZE_MEMBER(float, duration)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(float, pitch)
_END_PACKET

//S2CPacketSubGunFireCd
_BEGIN_PACKET_S2C(S2CPacketType::SubGunFireCd, DataPacket::emNormal, S2CPacketSubGunFireCd)
_SERIALIZE_MEMBER(int, gunId)
_SERIALIZE_MEMBER(float, fireCd)
_END_PACKET

//S2CPacketEntityHurtTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityHurtTime, DataPacket::emNormal, S2CPacketEntityHurtTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketLotteryData
_BEGIN_PACKET_S2C(S2CPacketType::LotteryData, DataPacket::emNormal, S2CPacketLotteryData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketLotteryResult
_BEGIN_PACKET_S2C(S2CPacketType::LotteryResult, DataPacket::emNormal, S2CPacketLotteryResult)
_SERIALIZE_MEMBER(String, firstId)
_SERIALIZE_MEMBER(String, secondId)
_SERIALIZE_MEMBER(String, thirdId)
_END_PACKET

//S2CPacketHideAndSeekHallResult
_BEGIN_PACKET_S2C(S2CPacketType::HideAndSeekHallResult, DataPacket::emNormal, S2CPacketHideAndSeekHallResult)
_SERIALIZE_MEMBER(String, itemInfo)
_END_PACKET

//S2CPacketEntityColorfulTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityColorfulTime, DataPacket::emNormal, S2CPacketEntityColorfulTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketEntitySelected
_BEGIN_PACKET_S2C(S2CPacketType::EntitySelected, DataPacket::emNormal, S2CPacketEntitySelected)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(int, type)
_END_PACKET

//S2CPacketSyncPlayerSetSpYaw
_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerSetSpYaw, DataPacket::emNormal, S2CPacketSyncPlayerSetSpYaw)
_SERIALIZE_MEMBER(i32, playerEntityId)
_SERIALIZE_MEMBER(bool, isSpYaw)
_SERIALIZE_MEMBER(float, radian)
_END_PACKET

//S2CPacketEntityDisableMoveTime
_BEGIN_PACKET_S2C(S2CPacketType::EntityDisableMoveTime, DataPacket::emNormal, S2CPacketEntityDisableMoveTime)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, time)
_END_PACKET

//S2CPacketLogicSettingInfo
_BEGIN_PACKET_S2C(S2CPacketType::LogicSettingInfo, DataPacket::emNormal, S2CPacketLogicSettingInfo)
_SERIALIZE_MEMBER(i32, m_mainInventorySize)
_SERIALIZE_MEMBER(vector<BLOCKMAN::GunPluginSetting>::type, m_gunSetting)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BulletClipPluginSetting>::type, m_bulletClipSetting)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BlockDynamicAttr>::type, m_blockDynamicAttr)
_SERIALIZE_MEMBER(vector<BLOCKMAN::ToolItemDynamicAttr>::type, m_toolItemDynamicAttr)
_SERIALIZE_MEMBER(bool8, m_sneakShowName)
_SERIALIZE_MEMBER(bool8, m_breakBlockSoon)
_SERIALIZE_MEMBER(bool8, m_isCanDamageItem)
_SERIALIZE_MEMBER(bool8, m_disableSelectEntity)
_SERIALIZE_MEMBER(bool, m_gunIsNeedBullet)
_SERIALIZE_MEMBER(bool, m_hideClouds)
_SERIALIZE_MEMBER(bool, m_canCloseChest)
_SERIALIZE_MEMBER(bool, m_showGunEffectWithSingle)
_SERIALIZE_MEMBER(bool, m_allowHeadshot)
_SERIALIZE_MEMBER(float, m_thirdPersonDistance)
_SERIALIZE_MEMBER(vector<BLOCKMAN::SkillItem>::type, m_skillItemSetting)
_END_PACKET

//S2CPacketSendOpenPixelGunHallModeSelect
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenPixelGunHallModeSelect, DataPacket::emNormal, S2CPacketSendOpenPixelGunHallModeSelect)
_SERIALIZE_MEMBER(bool, open)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketUpdateStoreGunData
_BEGIN_PACKET_S2C(S2CPacketType::UpdateStoreGunData, DataPacket::emNormal, S2CPacketUpdateStoreGunData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketUpdateStorePropData
_BEGIN_PACKET_S2C(S2CPacketType::UpdateStorePropData, DataPacket::emNormal, S2CPacketUpdateStorePropData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketSyncEffectGunFire
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFire, DataPacket::emNormal, S2CPacketSyncEffectGunFire)
_SERIALIZE_MEMBER(ui32, shootingEntityId)
_SERIALIZE_MEMBER(ui32, gunId)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, beginPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, endPos)
_END_PACKET

//S2CPacketSyncEffectGunFireAtEntityResult
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFireAtEntityResult, DataPacket::emNormal, S2CPacketSyncEffectGunFireAtEntityResult)
_SERIALIZE_MEMBER(ui32, shootingEntityId)
_SERIALIZE_MEMBER(ui32, hitEntityId)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, dir)
_SERIALIZE_MEMBER(ui32, traceType)
_SERIALIZE_MEMBER(bool, isHeadshot)
_END_PACKET

//S2CPacketSyncEffectGunFireAtBlockResult
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFireAtBlockResult, DataPacket::emNormal, S2CPacketSyncEffectGunFireAtBlockResult)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3i, blockPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, dir)
_END_PACKET

//S2CPacketSyncEffectGunFireExplosionResult
_BEGIN_PACKET_S2C(S2CPacketType::SyncEffectGunFireExplosionResult, DataPacket::emNormal, S2CPacketSyncEffectGunFireExplosionResult)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, hitPos)
_SERIALIZE_MEMBER(float, explosionRange)
_END_PACKET

//S2CPacketStopLaserGun
_BEGIN_PACKET_S2C(S2CPacketType::StopLaserGun, DataPacket::emNormal, S2CPacketStopLaserGun)
_SERIALIZE_MEMBER(i32, shootingEntityId)
_END_PACKET

//S2CPacketShowGunStore
_BEGIN_PACKET_S2C(S2CPacketType::ShowGunStore, DataPacket::emNormal, S2CPacketShowGunStore)
_END_PACKET

//S2CPacketShowPixelGunHallInfo
_BEGIN_PACKET_S2C(S2CPacketType::ShowPixelGunHallInfo, DataPacket::emNormal, S2CPacketShowPixelGunHallInfo)
_SERIALIZE_MEMBER(int, lv)
_SERIALIZE_MEMBER(int, cur_exp)
_SERIALIZE_MEMBER(int, max_exp)
_SERIALIZE_MEMBER(int, yaoshi)
_SERIALIZE_MEMBER(bool, is_max)
_END_PACKET

//S2CPacketSendOpenPixelGun1v1
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenPixelGun1v1, DataPacket::emNormal, S2CPacketSendOpenPixelGun1v1)
_SERIALIZE_MEMBER(bool, open)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketShowChestLottery
_BEGIN_PACKET_S2C(S2CPacketType::ShowChestLottery, DataPacket::emNormal, S2CPacketShowChestLottery)
_SERIALIZE_MEMBER(bool, open)
_SERIALIZE_MEMBER(int, curIntegral)
_SERIALIZE_MEMBER(int, needIntegral)
_END_PACKET

//S2CPacketSendOpenPixelGunRevive
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenPixelGunRevive, DataPacket::emNormal, S2CPacketSendOpenPixelGunRevive)
_SERIALIZE_MEMBER(bool, open)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketChestLotteryResult
_BEGIN_PACKET_S2C(S2CPacketType::ChestLotteryResult, DataPacket::emNormal, S2CPacketChestLotteryResult)
_SERIALIZE_MEMBER(int, rewardId)
_SERIALIZE_MEMBER(bool, hasGet)
_END_PACKET

//S2CPacketUpdateLotteryChestData
_BEGIN_PACKET_S2C(S2CPacketType::UpdateLotteryChestData, DataPacket::emNormal, S2CPacketUpdateLotteryChestData)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketShowPixelGunGamePerson
_BEGIN_PACKET_S2C(S2CPacketType::ShowPixelGunGamePerson, DataPacket::emNormal, S2CPacketShowPixelGunGamePerson)
_SERIALIZE_MEMBER(int, time)
_SERIALIZE_MEMBER(int, rank)
_SERIALIZE_MEMBER(int, kill_num)
_END_PACKET

//S2CPacketShowPixelGunGame1v1
_BEGIN_PACKET_S2C(S2CPacketType::ShowPixelGunGame1v1, DataPacket::emNormal, S2CPacketShowPixelGunGame1v1)
_SERIALIZE_MEMBER(int, time)
_SERIALIZE_MEMBER(int, red_kill_num)
_SERIALIZE_MEMBER(int, blue_kill_num)
_SERIALIZE_MEMBER(int, self_team)
_END_PACKET

//S2CPacketShowPixelGunGameTeam
_BEGIN_PACKET_S2C(S2CPacketType::ShowPixelGunGameTeam, DataPacket::emNormal, S2CPacketShowPixelGunGameTeam)
_SERIALIZE_MEMBER(int, time)
_SERIALIZE_MEMBER(int, red_kill_num)
_SERIALIZE_MEMBER(int, red_cur_player_num)
_SERIALIZE_MEMBER(int, red_max_player_num)
_SERIALIZE_MEMBER(int, blue_kill_num)
_SERIALIZE_MEMBER(int, blue_cur_player_num)
_SERIALIZE_MEMBER(int, blue_max_player_num)
_SERIALIZE_MEMBER(int, self_team)
_END_PACKET


//S2CPacketSendOpenArmorUpgrade
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenArmorUpgrade, DataPacket::emNormal, S2CPacketSendOpenArmorUpgrade)
_SERIALIZE_MEMBER(bool, open)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketSendOpenPixelGunResult
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenPixelGunResult, DataPacket::emNormal, S2CPacketSendOpenPixelGunResult)
_SERIALIZE_MEMBER(bool, open)
_SERIALIZE_MEMBER(String, data)
_SERIALIZE_MEMBER(int, result_type)
_END_PACKET

//S2CPacketPlayerCurrentSeasonInfo
_BEGIN_PACKET_S2C(S2CPacketType::PlayerCurrentSeasonInfo, DataPacket::emNormal, S2CPacketPlayerCurrentSeasonInfo)
_SERIALIZE_MEMBER(int, honorId)
_SERIALIZE_MEMBER(int, rank)
_SERIALIZE_MEMBER(int, honor)
_SERIALIZE_MEMBER(int, endTime)
_END_PACKET

//S2CPacketPlayerLastSeasonInfo
_BEGIN_PACKET_S2C(S2CPacketType::PlayerLastSeasonInfo, DataPacket::emNormal, S2CPacketPlayerLastSeasonInfo)
_SERIALIZE_MEMBER(int, honorId)
_SERIALIZE_MEMBER(int, rank)
_SERIALIZE_MEMBER(int, honor)
_END_PACKET

//S2CPacketEnableAutoShoot
_BEGIN_PACKET_S2C(S2CPacketType::EnableAutoShoot, DataPacket::emNormal, S2CPacketEnableAutoShoot)
_SERIALIZE_MEMBER(bool, enable)
_END_PACKET

//S2CPacketDisarmament
_BEGIN_PACKET_S2C(S2CPacketType::Disarmament, DataPacket::emNormal, S2CPacketDisarmament)
_SERIALIZE_MEMBER(bool, isDisarmament)
_END_PACKET

//S2CPacketSetPlayerOpacity
_BEGIN_PACKET_S2C(S2CPacketType::SetPlayerOpacity, DataPacket::emNormal, S2CPacketSetPlayerOpacity)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, opacityValue)
_END_PACKET

//S2CPacketSendOpenPixelGunLvUp
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenPixelGunLvUp, DataPacket::emNormal, S2CPacketSendOpenPixelGunLvUp)
_SERIALIZE_MEMBER(bool, open)
_SERIALIZE_MEMBER(String, data)
_END_PACKET

//S2CPacketSendOpenPixelGunResultSpecialInfo
_BEGIN_PACKET_S2C(S2CPacketType::SendOpenPixelGunResultSpecialInfo, DataPacket::emNormal, S2CPacketSendOpenPixelGunResultSpecialInfo)
_SERIALIZE_MEMBER(bool, btnRevengeEnable)
_END_PACKET

//S2CPacketNotifyGetGoods
_BEGIN_PACKET_S2C(S2CPacketType::NotifyGetGoods, DataPacket::emNormal, S2CPacketNotifyGetGoods)
_SERIALIZE_MEMBER(String, icon)
_SERIALIZE_MEMBER(i32, count)
_END_PACKET

//S2CPacketShowSeasonRank
_BEGIN_PACKET_S2C(S2CPacketType::ShowSeasonRank, DataPacket::emNormal, S2CPacketShowSeasonRank)
_END_PACKET

//S2CPacketPlayerNamePerspective
_BEGIN_PACKET_S2C(S2CPacketType::PlayerNamePerspective, DataPacket::emNormal, S2CPacketPlayerNamePerspective)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isPerspective)
_END_PACKET

//S2CPacketAddCustomEffect
_BEGIN_PACKET_S2C(S2CPacketType::AddCustomEffect, DataPacket::emNormal, S2CPacketAddCustomEffect)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(String, name)
_SERIALIZE_MEMBER(String, effectName)
_SERIALIZE_MEMBER(float, duration)
_END_PACKET

//S2CPacketSyncCustomEffect
_BEGIN_PACKET_S2C(S2CPacketType::SyncCustomEffect, DataPacket::emNormal, S2CPacketSyncCustomEffect)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(vector<BLOCKMAN::CustomEffect>::type, customEffects)
_END_PACKET

//S2CPacketSyncEntityBulletin
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBulletin, DataPacket::emNormal, S2CPacketSyncEntityBulletin)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(i32, bulletinId)
_END_PACKET

//S2CPacketSyncEntityBird
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityBird, DataPacket::emNormal, S2CPacketSyncEntityBird)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(ui64, userId)
_SERIALIZE_MEMBER(i64, birdId)
_SERIALIZE_MEMBER(String, nameLang)
_SERIALIZE_MEMBER(String, actorName)
_SERIALIZE_MEMBER(String, actorBody)
_SERIALIZE_MEMBER(String, actorBodyId)
_SERIALIZE_MEMBER(String, dressGlasses)
_SERIALIZE_MEMBER(String, dressHat)
_SERIALIZE_MEMBER(String, dressBeak)
_SERIALIZE_MEMBER(String, dressWing)
_SERIALIZE_MEMBER(String, dressTail)
_SERIALIZE_MEMBER(String, dressEffect)
_END_PACKET

//S2CPacketSyncPlayerBagInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncPlayerBagInfo, DataPacket::emNormal, S2CPacketSyncPlayerBagInfo)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(bool, isConvert)
_SERIALIZE_MEMBER(i32, curCapacity)
_SERIALIZE_MEMBER(i32, maxCapacity)
_END_PACKET

//S2CPacketSyncBirdStore
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdStore, DataPacket::emNormal, S2CPacketSyncBirdStore)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdStore>::type, stores)
_END_PACKET

//S2CPacketSyncBirdStore
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdPersonalStore, DataPacket::emNormal, S2CPacketSyncBirdPersonalStore)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdPersonalStoreTab>::type, personalStore)
_END_PACKET

//S2CPacketSyncBag
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdBag, DataPacket::emNormal, S2CPacketSyncBirdBag)
_SERIALIZE_MEMBER(BLOCKMAN::BirdBag, birdBag)
_END_PACKET

//S2CPacketSyncBirdPlayerInfo
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdPlayerInfo, DataPacket::emNormal, S2CPacketSyncBirdPlayerInfo)
_SERIALIZE_MEMBER(BLOCKMAN::BirdPlayerInfo, playerInfo)
_END_PACKET

//S2CPacketSyncBirdDress
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdDress, DataPacket::emNormal, S2CPacketSyncBirdDress)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdDress>::type, dress)
_END_PACKET

//S2CPacketSyncBirdFood
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdFood, DataPacket::emNormal, S2CPacketSyncBirdFood)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdFood>::type, foods)
_END_PACKET

//S2CPacketSyncBirdTask
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdTask, DataPacket::emNormal, S2CPacketSyncBirdTask)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdTask>::type, tasks)
_END_PACKET

//S2CPacketSyncBirdAtlas
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdAtlas, DataPacket::emNormal, S2CPacketSyncBirdAtlas)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdAtlas>::type, atlas)
_END_PACKET

//S2CPacketBirdGain
_BEGIN_PACKET_S2C(S2CPacketType::BirdGain, DataPacket::emNormal, S2CPacketBirdGain)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdGain>::type, items)
_END_PACKET

//S2CPacketBirdNestOperation
_BEGIN_PACKET_S2C(S2CPacketType::BirdNestOperation, DataPacket::emNormal, S2CPacketBirdNestOperation)
_SERIALIZE_MEMBER(i64, birdId)
_SERIALIZE_MEMBER(bool, isUnlock)
_END_PACKET

//S2CPacketSyncBirdScope
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdScope, DataPacket::emNormal, S2CPacketSyncBirdScope)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdScope>::type, scopes)
_END_PACKET

//S2CPacketSyncBirdActivity
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdActivity, DataPacket::emNormal, S2CPacketSyncBirdActivity)
_SERIALIZE_MEMBER(BLOCKMAN::BirdGiftBag, activity)
_END_PACKET


//S2CPacketBirdLotteryResult
_BEGIN_PACKET_S2C(S2CPacketType::BirdLotteryResult, DataPacket::emNormal, S2CPacketBirdLotteryResult)
_SERIALIZE_MEMBER(String, eggBodyId)
_SERIALIZE_MEMBER(BLOCKMAN::BirdInfo, birdInfo)
_END_PACKET


//S2CPacketSyncEntityAction
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityAction, DataPacket::emNormal, S2CPacketSyncEntityAction)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(int, entityClass)
_SERIALIZE_MEMBER(int, actionStatus)
_SERIALIZE_MEMBER(int, baseAction)
_SERIALIZE_MEMBER(int, preBaseAction)
_END_PACKET

//S2CPacketSyncBirdTimePrice
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdTimePrice, DataPacket::emNormal, S2CPacketSyncBirdTimePrice)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdTimePrice>::type, timePrices)
_END_PACKET

//S2CPacketSyncBirdAddScore
_BEGIN_PACKET_S2C(S2CPacketType::BirdAddScore, DataPacket::emNormal, S2CPacketBirdAddScore)
_SERIALIZE_MEMBER(i32, score)
_SERIALIZE_MEMBER(i32, scoreType)
_END_PACKET


//S2CPacketSyncBirdBuff
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdBuff, DataPacket::emNormal, S2CPacketSyncBirdBuff)
_SERIALIZE_MEMBER(vector<BLOCKMAN::BirdBuff>::type, buffs)
_END_PACKET

//S2CPacketSyncEntityHealth
_BEGIN_PACKET_S2C(S2CPacketType::SyncEntityHealth, DataPacket::emNormal, S2CPacketSyncEntityHealth)
_SERIALIZE_MEMBER(i32, entityId)
_SERIALIZE_MEMBER(float, health)
_SERIALIZE_MEMBER(float, maxHealth)
_END_PACKET

//S2CPacketSyncBirdSimulatorStoreItem
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdSimulatorStoreItem, DataPacket::emNormal, S2CPacketSyncBirdSimulatorStoreItem)
_SERIALIZE_MEMBER(BLOCKMAN::BirdStore, store)
_END_PACKET

//S2CPacketSyncBirdSimulatorTaskItem
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdSimulatorTaskItem, DataPacket::emNormal, S2CPacketSyncBirdSimulatorTaskItem)
_SERIALIZE_MEMBER(bool, isRemove)
_SERIALIZE_MEMBER(BLOCKMAN::BirdTask, taskItem)
_END_PACKET

//S2CPacketSyncBirdSimulatorPersonStoreTab
_BEGIN_PACKET_S2C(S2CPacketType::SyncBirdSimulatorPersonStoreTab, DataPacket::emNormal, S2CPacketSyncBirdSimulatorPersonStoreTab)
_SERIALIZE_MEMBER(BLOCKMAN::BirdPersonalStoreTab, storeTab)
_END_PACKET

//S2CPacketChangeGuideArrowStatus
_BEGIN_PACKET_S2C(S2CPacketType::ChangeGuideArrowStatus, DataPacket::emNormal, S2CPacketChangeGuideArrowStatus)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, arrowPos)
_SERIALIZE_MEMBER(bool, flag)
_END_PACKET

//S2CPacketAddWallText
_BEGIN_PACKET_S2C(S2CPacketType::AddWallText, DataPacket::emNormal, S2CPacketAddWallText)
_SERIALIZE_MEMBER(String, text)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, textPos)
_SERIALIZE_MEMBER(float, scale)
_SERIALIZE_MEMBER(float, yaw)
_SERIALIZE_MEMBER(float, pitch)
_SERIALIZE_MEMBER(float, r)
_SERIALIZE_MEMBER(float, g)
_SERIALIZE_MEMBER(float, b)
_SERIALIZE_MEMBER(float, a)
_END_PACKET

//S2CPacketDeleteWallText
_BEGIN_PACKET_S2C(S2CPacketType::DeleteWallText, DataPacket::emNormal, S2CPacketDeleteWallText)
_SERIALIZE_MEMBER(S2CPACKET_DETAIL::Vector3, textPos)
_END_PACKET

// ============================================================================
// Server-authoritative world generation packets (see docs/WORLDGEN.md).
// These ship chunk data + world info from the server to the client. The client
// NEVER runs WorldGenerator — it only renders what these packets deliver.
// ============================================================================

//S2CPacketWorldInfo  — sent once on connect, before any chunk data.
_BEGIN_PACKET_S2C(S2CPacketType::WorldInfo, DataPacket::emMapData, S2CPacketWorldInfo)
_SERIALIZE_MEMBER(ui64, m_worldSeedHash)        // hash of seed; raw seed never leaves the server
_SERIALIZE_MEMBER(i32, m_spawnX)
_SERIALIZE_MEMBER(i32, m_spawnY)
_SERIALIZE_MEMBER(i32, m_spawnZ)
_SERIALIZE_MEMBER(ui8, m_gameType)              // 0=survival, 1=creative, 2=adventure, 3=spectator
_SERIALIZE_MEMBER(ui8, m_worldType)             // 0=default, 1=flat, 2=large biomes, 3=amplified
_SERIALIZE_MEMBER(ui8, m_dimension)             // 0=overworld, -1=nether, 1=end (sent as ui8)
_END_PACKET

//S2CPacketChunkData  — full chunk payload.
//Wire format: chunkX, chunkZ, then a raw byte blob containing the serialised
//sections + biomes + heightmap + block-entities + entities. The blob is
//produced by the server's chunk encoder (Anvil-compatible NBT + palette).
//The client decodes by passing the blob to the same ChunkReadableStorageFile
//decoder used for region files, so disk and wire share one decoder.
_BEGIN_PACKET_S2C(S2CPacketType::ChunkData, DataPacket::emMapData, S2CPacketChunkData)
_SERIALIZE_MEMBER(i32, m_chunkX)
_SERIALIZE_MEMBER(i32, m_chunkZ)
_SERIALIZE_MEMBER(LORD::vector<ui8>::type, m_blob)
_END_PACKET

//S2CPacketBlockChange  — a single block changed (player edit or server event).
_BEGIN_PACKET_S2C(S2CPacketType::BlockChange, DataPacket::emMapData, S2CPacketBlockChange)
_SERIALIZE_MEMBER(i32, m_blockX)
_SERIALIZE_MEMBER(i32, m_blockY)
_SERIALIZE_MEMBER(i32, m_blockZ)
_SERIALIZE_MEMBER(ui16, m_blockId)
_SERIALIZE_MEMBER(ui8,  m_blockMeta)
_END_PACKET

//S2CPacketMultiBlockChange  — a batch of block changes within one chunk.
//Layout: chunkX, chunkZ, then a byte blob of (localX, localY, localZ, blockId, blockMeta) records.
_BEGIN_PACKET_S2C(S2CPacketType::MultiBlockChange, DataPacket::emMapData, S2CPacketMultiBlockChange)
_SERIALIZE_MEMBER(i32, m_chunkX)
_SERIALIZE_MEMBER(i32, m_chunkZ)
_SERIALIZE_MEMBER(LORD::vector<ui8>::type, m_blob)
_END_PACKET

#endif // !__S2C_PACKTS_H__
