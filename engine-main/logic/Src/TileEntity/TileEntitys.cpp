#include "TileEntitys.h"

#include "Item/ItemStack.h"
#include "World/World.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/Recipes.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Block/BM_Material.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{
	
TileEntityEnderChest::TileEntityEnderChest()
	: TileEntity(TILE_ENTITY_ENDERCHEST)
	, m_ticksSync(0)
	, m_lidAngle(0.f)
	, m_prevLidAngle(0.f)
	, m_numUsingPlayers(0)
{
}

TileEntityEnderChest::~TileEntityEnderChest()
{}

void TileEntityEnderChest::updateEntity()
{
	TileEntity::updateEntity();

	if (++m_ticksSync % 20 * 4 == 0)
	{
		worldObj->addBlockEvent(m_pos, BLOCK_ID_ENDER_CHEST, 1, m_numUsingPlayers);
	}

	m_prevLidAngle = m_lidAngle;
	float angle = 0.1F;

	if (m_numUsingPlayers > 0 && m_lidAngle == 0.0F)
	{
		//worldObj->playSoundEffect(Vector3(m_pos) + 0.5f, "random.chestopen", 0.5F, worldObj->m_Rand.nextFloat() * 0.1F + 0.9F);
		worldObj->playSoundEffectByType(Vector3(m_pos) + 0.5f, ST_OpenChest);
	}

	if (m_numUsingPlayers == 0 && m_lidAngle > 0.0F || m_numUsingPlayers > 0 && m_lidAngle < 1.0F)
	{
		float preAngle = m_lidAngle;

		if (m_numUsingPlayers > 0)
		{
			m_lidAngle += angle;
		}
		else
		{
			m_lidAngle -= angle;
		}

		if (m_lidAngle > 1.0F)
		{
			m_lidAngle = 1.0F;
		}

		float var3 = 0.5F;

		if (m_lidAngle < var3 && preAngle >= var3)
		{
			//worldObj->playSoundEffect(Vector3(m_pos) + 0.5f, "random.chestclosed", 0.5f, worldObj->m_Rand.nextFloat() * 0.1F + 0.9F);
			worldObj->playSoundEffectByType(Vector3(m_pos) + 0.5f, ST_CloseChest);
		}

		if (m_lidAngle < 0.0F)
		{
			m_lidAngle = 0.0F;
		}
	}
}

bool TileEntityEnderChest::receiveClientEvent(int id, int type)
{
	if (id == 1)
	{
		m_numUsingPlayers = type;
		return true;
	}
	else
	{
		return TileEntity::receiveClientEvent(id, type);
	}
}

void TileEntityEnderChest::invalidate()
{
	updateContainingBlockInfo();
	TileEntity::invalidate();
}

void TileEntityEnderChest::openChest()
{
	++m_numUsingPlayers;
	worldObj->addBlockEvent(m_pos, BLOCK_ID_ENDER_CHEST, 1, m_numUsingPlayers);
}

void TileEntityEnderChest::closeChest()
{
	--m_numUsingPlayers;
	worldObj->addBlockEvent(m_pos, BLOCK_ID_ENDER_CHEST, 1, m_numUsingPlayers);
}

bool TileEntityEnderChest::isUseableByPlayer(EntityPlayer* pPlayer)
{
	return worldObj->getBlockTileEntity(m_pos) != this ? false :
		pPlayer->getDistanceSq(Vector3(m_pos) + 0.5f) <= 64.0f;
}

int TileEntityEnderChest::getChestType()
{
	if (!worldObj)
		return 0;

	BlockEnderChest* pbkEnderChest = dynamic_cast<BlockEnderChest*>(getBlock());
	if (!pbkEnderChest)
		return 0;

	return pbkEnderChest->getChestType();
}

TileEntityPiston::TileEntityPiston()
	: TileEntity(TILE_ENTITY_PISTON)
	, m_blockID(0)
	, m_meta(0)
	, m_dir(0)
	, m_extending(false)
	, m_renderHead(false)
	, m_progress(0.f)
	, m_lastProgress(0.f)
{}

TileEntityPiston::TileEntityPiston(int pistonID, int meta, int dir, bool bExtending, bool renderHead)
	: TileEntity(TILE_ENTITY_PISTON)
	, m_blockID(pistonID)
	, m_meta(meta)
	, m_dir(dir)
	, m_extending(bExtending)
	, m_renderHead(renderHead)
	, m_progress(0.f)
	, m_lastProgress(0.f)
{
}

TileEntityPiston::~TileEntityPiston()
{}

float TileEntityPiston::getProgress(float rdt)
{
	if (rdt > 1.0F)
	{
		rdt = 1.0F;
	}

	return m_lastProgress + (m_progress - m_lastProgress) * rdt;
}

float TileEntityPiston::getOffsetX(float rdt)
{
	if (m_extending)
		return (getProgress(rdt) - 1.0F) * (float)offsetXForSide[m_dir];
	else
		return (1.0F - getProgress(rdt)) * (float)offsetXForSide[m_dir];
}

float TileEntityPiston::getOffsetY(float rdt)
{
	if (m_extending)
		return (getProgress(rdt) - 1.0F) * (float)offsetYForSide[m_dir];
	else
		return (1.0F - getProgress(rdt)) * (float)offsetYForSide[m_dir];
}

float TileEntityPiston::getOffsetZ(float rdt)
{
	if (m_extending)
		return (getProgress(rdt) - 1.0F) * (float)offsetZForSide[m_dir];
	else
		return (1.0F - getProgress(rdt)) * (float)offsetZForSide[m_dir];
}

Vector3 TileEntityPiston::getOffset(float rdt)
{
	Vector3 ret;
	ret.x = getOffsetX(rdt);
	ret.y = getOffsetY(rdt);
	ret.z = getOffsetZ(rdt);
	return ret;
}

void TileEntityPiston::updatePushedObjects(float progress, float dis)
{
	if (m_extending)
	{
		progress = 1.0F - progress;
	}
	else
	{
		--progress;
	}

	Box aabb = BlockManager::pistonMoving->getAxisAlignedBB(worldObj, m_pos, m_blockID, progress, m_dir);

	if (!aabb.isNull())
	{
		EntityArr entities = worldObj->getEntitiesWithinAABBExcludingEntity(NULL, aabb);

		if (!entities.empty())
		{
			m_pushedObjects.insert(m_pushedObjects.end(), entities.begin(), entities.end());

			for (auto it : m_pushedObjects)
			{
				Entity* pEntity = it;
				Vector3 moveVec = Vector3(offsetForSide[m_dir]) * dis;
				pEntity->moveEntity(moveVec);
			}

			m_pushedObjects.clear();
		}
	}
}

void TileEntityPiston::clearPistonTileEntity()
{
	if (m_lastProgress < 1.0F && worldObj)
	{
		m_lastProgress = m_progress = 1.0F;
		worldObj->removeBlockTileEntity(m_pos);
		invalidate();

		if (worldObj->getBlockId(m_pos) == BLOCK_ID_PISTON_MOVING)
		{
			worldObj->setBlock(m_pos, m_blockID, m_meta, 3);
			worldObj->notifyBlockOfNeighborChange(m_pos, m_blockID);
		}
	}
}

void TileEntityPiston::updateEntity()
{
	m_lastProgress = m_progress;

	if (m_lastProgress >= 1.0F)
	{
		updatePushedObjects(1.0F, 0.25F);
		worldObj->removeBlockTileEntity(m_pos);
		invalidate();

		if (worldObj->getBlockId(m_pos) == BLOCK_ID_PISTON_MOVING)
		{
			worldObj->setBlock(m_pos, m_blockID, m_meta, 3);
			worldObj->notifyBlockOfNeighborChange(m_pos, m_blockID);
		}
	}
	else
	{
		m_progress += 0.5F;

		if (m_progress >= 1.0F)
		{
			m_progress = 1.0F;
		}

		if (m_extending)
		{
			updatePushedObjects(m_progress, m_progress - m_lastProgress + 0.0625F);
		}
	}
}

void TileEntityPiston::readFromNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::readFromNBT(pNbtCompound);
	m_blockID = pNbtCompound->getInteger("blockId");
	m_meta = pNbtCompound->getInteger("blockData");
	m_dir = pNbtCompound->getInteger("facing");
	m_lastProgress = m_progress = pNbtCompound->getFloat("progress");
	m_extending = pNbtCompound->getBool("extending");
}

void TileEntityPiston::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);
	pNbtCompound->setInteger("blockId", m_blockID);
	pNbtCompound->setInteger("blockData", m_meta);
	pNbtCompound->setInteger("facing", m_dir);
	pNbtCompound->setFloat("progress", m_lastProgress);
	pNbtCompound->setBool("extending", m_extending);
}

TileEntityComparator::TileEntityComparator()
	: TileEntity(TILE_ENTITY_COMPARATOR)
	, outputSignal(0)
{}

TileEntityComparator::~TileEntityComparator()
{}

void TileEntityComparator::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);
	pNbtCompound->setInteger("OutputSignal", outputSignal);
}

void TileEntityComparator::readFromNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::readFromNBT(pNbtCompound);
	outputSignal = pNbtCompound->getInteger("OutputSignal");
}

TileEntitySkull::TileEntitySkull()
	: TileEntity(TILE_ENTITY_SKULL)
	, m_skullType(0)
	, m_skullRotation(0)
{}

TileEntitySkull::~TileEntitySkull()
{}

void TileEntitySkull::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);
	pNbtCompound->setByte("SkullType", (i8)(m_skullType & 255));
	pNbtCompound->setByte("Rot", (i8)(m_skullRotation & 255));
	pNbtCompound->setString("ExtraType", extraType);
}

void TileEntitySkull::readFromNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::readFromNBT(pNbtCompound);
	m_skullType = pNbtCompound->getByte("SkullType");
	m_skullRotation = pNbtCompound->getByte("Rot");

	if (pNbtCompound->hasKey("ExtraType"))
	{
		extraType = pNbtCompound->getString("ExtraType");
	}
}

TileEntityRecordPlayer::TileEntityRecordPlayer()
	: TileEntity(TILE_ENTITY_RECORDPLAYER)
{}

TileEntityRecordPlayer::~TileEntityRecordPlayer()
{

}

void TileEntityRecordPlayer::readFromNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::readFromNBT(pNbtCompound);

	if (pNbtCompound->hasKey("RecordItem"))
	{
		setRecord(ItemStack::loadItemStackFromNBT(pNbtCompound->getCompoundTag("RecordItem")));
	}
	else if (pNbtCompound->getInteger("Record") > 0)
	{
		setRecord(LORD::make_shared<ItemStack>(pNbtCompound->getInteger("Record"), 1, 0));
	}
}

void TileEntityRecordPlayer::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);

	if (getRecord() != NULL)
	{
		pNbtCompound->setCompoundTag("RecordItem", record->writeToNBT(LordNew NBTTagCompound()));
		pNbtCompound->setInteger("Record", record->itemID);
	}
}

void TileEntityRecordPlayer::setRecord(ItemStackPtr pStack)
{
	record = pStack;
	onInventoryChanged();
}

TileEntitySign::TileEntitySign()
	: TileEntity(TILE_ENTITY_SIGN)
	, m_lineEdited(-1)
	, m_isEditable(true)
	, m_player(NULL)
{
}

TileEntitySign::~TileEntitySign()
{}

void TileEntitySign::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);
	pNbtCompound->setString("Text1", m_signTxt[0]);
	pNbtCompound->setString("Text2", m_signTxt[1]);
	pNbtCompound->setString("Text3", m_signTxt[2]);
	pNbtCompound->setString("Text4", m_signTxt[3]);
}

void TileEntitySign::readFromNBT(NBTTagCompound* pNbtCompound)
{
	m_isEditable = false;
	TileEntity::readFromNBT(pNbtCompound);

	for (int i = 0; i < SIGN_COUNT; ++i)
	{
		m_signTxt[i] = pNbtCompound->getString(StringUtil::Format("Text%d", i + 1));

		if (m_signTxt[i].length() > 15)
		{
			m_signTxt[i] = m_signTxt[i].substr(0, 15);
		}
	}
}

void TileEntitySign::setEditable(bool editable)
{
	m_isEditable = editable;

	if (!editable)
	{
		m_player = NULL;
	}
}

TileEntityMobSpawner::TileEntityMobSpawner()
	: TileEntity(TILE_ENTITY_MOBSWPAWER)
{}

TileEntityMobSpawner::~TileEntityMobSpawner()
{}

void TileEntityMobSpawner::readFromNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::readFromNBT(pNbtCompound);
	// m_spawnLogic.readFromNBT(pNbtCompound);
}
void TileEntityMobSpawner::writeToNBT(NBTTagCompound* pNbtCompound)
{
	TileEntity::writeToNBT(pNbtCompound);
	// m_spawnLogic.writeToNBT(par1NBTTagCompound);
}

void TileEntityMobSpawner::updateEntity()
{
	// m_spawnLogic.updateSpawner();
	TileEntity::updateEntity();
}

bool TileEntityMobSpawner::receiveClientEvent(int id, int type)
{
	// todo.
	// return m_spawnLogic.setDelayToMin(id) ? true : super.receiveClientEvent(id, type);
	return false;
}

TileEntityNote::TileEntityNote()
	: TileEntity(TILE_ENTITY_MUSIC)
	, m_note(0)
	, m_prevRedstoneState(false)
{}

TileEntityNote::~TileEntityNote()
{}

void TileEntityNote::writeToNBT(NBTTagCompound* pNbt)
{
	TileEntity::writeToNBT(pNbt);
	pNbt->setByte("note", m_note);
}

void TileEntityNote::readFromNBT(NBTTagCompound* pNbt)
{
	TileEntity::readFromNBT(pNbt);
	m_note = pNbt->getByte("note");

	m_note = Math::Clamp(m_note, i8(0), i8(24));
}

void TileEntityNote::changePitch()
{
	m_note = (m_note + 1) % 25;
	onInventoryChanged();
}

void TileEntityNote::triggerNote(World* pWorld, const BlockPos& pos)
{
	if (pWorld->getBlockMaterial(pos.getPosY()) == BM_Material::BM_MAT_air)
	{
		const BM_Material& mat = pWorld->getBlockMaterial(pos.getNegY());
		i8 event = 0;

		if (mat == BM_Material::BM_MAT_rock)
			event = 1;
		if (mat == BM_Material::BM_MAT_sand)
			event = 2;
		if (mat == BM_Material::BM_MAT_glass)
			event = 3;
		if (mat == BM_Material::BM_MAT_wood)
			event = 4;

		pWorld->addBlockEvent(pos, BlockManager::music->getBlockID(), event, m_note);
	}
}

Random TileEntityEnchantmentTable::rand(0LL);

TileEntityEnchantmentTable::TileEntityEnchantmentTable()
	: TileEntity(TILE_ENTITY_ENCHANTTABLE)
	, m_tickCount(0)
	, m_pageFlip(0.f)
	, m_pageFlipPrev(0.f)
	, m_flipT(0.f)
	, m_flipA(0.f)
	, m_bookSpread(0.f)
	, m_bookSpreadPrev(0.f)
	, m_bookRotation2(0.f)
	, m_bookRotationPrev(0.f)
	, m_bookRotation(0.f)
{}

TileEntityEnchantmentTable::~TileEntityEnchantmentTable()
{}

void TileEntityEnchantmentTable::writeToNBT(NBTTagCompound* pNbt)
{
	TileEntity::writeToNBT(pNbt);

	if (hasCustomName())
	{
		pNbt->setString("CustomName", m_customName);
	}
}

void TileEntityEnchantmentTable::readFromNBT(NBTTagCompound* pNbt)
{
	TileEntity::readFromNBT(pNbt);

	if (pNbt->hasKey("CustomName"))
	{
		m_customName = pNbt->getString("CustomName");
	}
}

void TileEntityEnchantmentTable::updateEntity()
{
	TileEntity::updateEntity();
	m_bookSpreadPrev = m_bookSpread;
	m_bookRotationPrev = m_bookRotation2;
	EntityPlayer* pPlayer = worldObj->getClosestPlayer(Vector3(m_pos) + Vector3::HALF, 3.0f);

	if (pPlayer)
	{
		float dx = pPlayer->position.x - (m_pos.x + 0.5F);
		float dz = pPlayer->position.z - (m_pos.z + 0.5F);
		m_bookRotation = Math::ATan2(dz, dx);
		m_bookSpread += 0.1F;

		if (m_bookSpread < 0.5F || rand.nextInt(40) == 0)
		{
			float var6 = m_flipT;

			do
			{
				m_flipT += (float)(rand.nextInt(4) - rand.nextInt(4));
			} while (var6 == m_flipT);
		}
	}
	else
	{
		m_bookRotation += 0.02F;
		m_bookSpread -= 0.1F;
	}

	while (m_bookRotation2 >= Math::PI)
	{
		m_bookRotation2 -= (Math::PI_2);
	}

	while (m_bookRotation2 < -Math::PI)
	{
		m_bookRotation2 += (Math::PI_2);
	}

	while (m_bookRotation >= Math::PI)
	{
		m_bookRotation -= (Math::PI_2);
	}

	while (m_bookRotation < -Math::PI)
	{
		m_bookRotation += (Math::PI_2);
	}

	float rot;

	for (rot = m_bookRotation - m_bookRotation2; rot >= Math::PI; rot -= (Math::PI_2))
	{
		;
	}

	while (rot < -Math::PI)
	{
		rot += (Math::PI_2);
	}

	m_bookRotation2 += rot * 0.4F;

	if (m_bookSpread < 0.0F)
	{
		m_bookSpread = 0.0F;
	}

	if (m_bookSpread > 1.0F)
	{
		m_bookSpread = 1.0F;
	}

	++m_tickCount;
	m_pageFlipPrev = m_pageFlip;
	float var3 = (m_flipT - m_pageFlip) * 0.4F;
	float var8 = 0.2F;

	if (var3 < -var8)
	{
		var3 = -var8;
	}

	if (var3 > var8)
	{
		var3 = var8;
	}

	m_flipA += (var3 - m_flipA) * 0.9F;
	m_pageFlip += m_flipA;
}

TileEntityEndPortal::TileEntityEndPortal()
	: TileEntity(TILE_ENTITY_AIRPORTAL)
{}

TileEntityEndPortal::~TileEntityEndPortal()
{}

TileEntityCommandBlock::TileEntityCommandBlock()
	: TileEntity(TILE_ENTITY_CONTROL)
	, m_succesCount(0)
{
	m_commandSenderName = "@";
}

TileEntityCommandBlock::~TileEntityCommandBlock()
{}

void TileEntityCommandBlock::setCommand(const String& cmd)
{
	m_command = cmd;
	onInventoryChanged();
}

int TileEntityCommandBlock::executeCommandOnPowered(World* pWorld)
{
	if (pWorld->m_isClient)
		return 0;

	return 0;
}

void TileEntityCommandBlock::writeToNBT(NBTTagCompound* pNbt)
{
	TileEntity::writeToNBT(pNbt);
	pNbt->setString("Command", m_command);
	pNbt->setInteger("SuccessCount", m_succesCount);
	pNbt->setString("CustomName", m_commandSenderName);
}

void TileEntityCommandBlock::readFromNBT(NBTTagCompound* pNbt)
{
	TileEntity::readFromNBT(pNbt);
	m_command = pNbt->getString("Command");
	m_succesCount = pNbt->getInteger("SuccessCount");

	if (pNbt->hasKey("CustomName"))
		m_commandSenderName = pNbt->getString("CustomName");
}

TileEntityDaylightDetector::TileEntityDaylightDetector()
	: TileEntity(TILE_ENTITY_DLDETECTOR)
{}

TileEntityDaylightDetector::~TileEntityDaylightDetector()
{}

void TileEntityDaylightDetector::updateEntity()
{
	if (worldObj && !worldObj->m_isClient && worldObj->getTotalWorldTime() % 20L == 0L)
	{
		blockType = getBlock();

		if (blockType && blockType->getBlockID() == BLOCK_ID_DAYLIGHT_DETECTOR)
		{
			// todo in client side.
			// ((BlockDaylightDetector*)blockType)->updateLightLevel(worldObj, m_pos);
		}
	}
}

}