#include "TileEntityInventory.h"
#include "TileEntity.h"
#include "TileEntitys.h"
#include "BM_TypeDef.h"

#include "Nbt/NBT.h"
#include "World/World.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"

namespace BLOCKMAN
{

TileEntity::IDTileEntityType TileEntity::tileEntityIds;
TileEntity::NameTileEntityType TileEntity::tileEntityNames;

TileEntity::TileEntity(int id)
	: m_id(id)
	, m_pos(Vector3i::ZERO)
	, blockMetadata(-1)
	, blockType(NULL)
	, worldObj(NULL)
	, tileEntityInvalid(false)
{
}

TileEntity::~TileEntity()
{}

void TileEntity::initialize()
{
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_FURNACE, "Furnace"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_CHEST, "Chest"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_ENDERCHEST, "EnderChest"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_RECORDPLAYER, "RecordPlayer"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_TRAP, "Trap"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_DROPPER, "Dropper"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_SIGN, "Sign"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_MOBSWPAWER, "MobSpawner"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_MUSIC, "Music"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_PISTON, "Piston"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_CAULDRON, "Cauldron"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_ENCHANTTABLE, "EnchantTable"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_AIRPORTAL, "Airportal"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_CONTROL, "Control"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_BEACON, "Beacon"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_SKULL, "Skull"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_DLDETECTOR, "DLDetector"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_HOPPER, "Hopper"));
	tileEntityIds.insert(std::make_pair(TILE_ENTITY_COMPARATOR, "Comparator"));

	for (auto it : tileEntityIds)
	{
		tileEntityNames.insert(std::make_pair(it.second, it.first));
	}
}

void TileEntity::readFromNBT(NBTTagCompound* pNBT)
{
	LordAssert(pNBT);
	m_pos.x = pNBT->getInteger("x");
	m_pos.y = pNBT->getInteger("y");
	m_pos.z = pNBT->getInteger("z");
}

void TileEntity::updateContainingBlockInfo()
{
	blockType = NULL;
	blockMetadata = -1;
}

const String& TileEntity::getClassName() const
{
	auto it = tileEntityIds.find(m_id);
	if (it != tileEntityIds.end())
		return it->second;

	LordAssert(false);
	return StringUtil::BLANK;
}

TileEntity * TileEntity::deserializeTileEntity(const String & id, NbtParser & parser, bool startWithCompoundTag)
{
	TileEntity* ret = nullptr;
	if (id == "sign")
	{
		std::unique_ptr<TileEntitySign> tileEntity(LordNew TileEntitySign);
		if (!deserialize(tileEntity.get(), parser, startWithCompoundTag))
		{
			return nullptr;
		}
		ret = tileEntity.release();
	}
	return ret;
}

bool TileEntity::serializeTileEntity(NbtGenerator & generator) const
{
	switch (m_id)
	{
	case TILE_ENTITY_SIGN:
		serialize(static_cast<const TileEntitySign*>(this), generator);
		return true;
	default:
		return false;
	}
}

bool TileEntity::serializable() const
{
	switch (m_id)
	{
	case TILE_ENTITY_SIGN:
		return true;
	default:
		return false;
	}
}

int TileEntity::getSuperClassID(int classID)
{
	deque<const RuntimeClass*>::type searchQueue;
	searchQueue.push_back(&rttiTileEntity);
	while (!searchQueue.empty())
	{
		const RuntimeClass* rtti = searchQueue.front();
		searchQueue.pop_front();
		if (rtti->m_classID == classID)
		{
			if (rtti->m_parent)
				return rtti->m_parent->m_classID;
			return 0;
		}

		for (int i = 0; i < int(rtti->m_children.size()); ++i)
			searchQueue.push_back(rtti->m_children[i]);
	}
	return 0;
}

/**
* Writes a tile entity to NBT.
*/
void TileEntity::writeToNBT(NBTTagCompound* pNBT)
{
	const String& classname = getClassName();

	pNBT->setString("id", classname);
	pNBT->setInteger("x", m_pos.x);
	pNBT->setInteger("y", m_pos.y);
	pNBT->setInteger("z", m_pos.z);
}

TileEntity* TileEntity::createAndLoadEntity(NBTTagCompound* pNBT)
{
	String classname = pNBT->getString("id");

	auto it = tileEntityNames.find(classname);
	if (it == tileEntityNames.end())
	{
		LordLogError("Error in TileEntity::createAndLoadEntity class name: %s", classname.c_str());
		return NULL;
	}

	TILE_ENTITY teType = (TILE_ENTITY)(it->second);
	if (!(teType > TILE_ENTITY_INVALID && teType < TILE_ENTITY_COUNT))
	{
		LordLogError("Error in TileEntity::createAndLoadEntity class name: %s", classname.c_str());
		return NULL;
	}

	TileEntity* pTileEntity = NULL;

	switch (teType)
	{
	case BLOCKMAN::TILE_ENTITY_FURNACE:
		pTileEntity = LordNew TileEntityFurnace();
		break;
	case BLOCKMAN::TILE_ENTITY_CHEST:
		pTileEntity = LordNew TileEntityChest();
		break;
	case BLOCKMAN::TILE_ENTITY_ENDERCHEST:
		pTileEntity = LordNew TileEntityEnderChest();
		break;
	case BLOCKMAN::TILE_ENTITY_RECORDPLAYER:
		pTileEntity = LordNew TileEntityRecordPlayer();
		break;
	case BLOCKMAN::TILE_ENTITY_TRAP:
		pTileEntity = LordNew TileEntityDispenser();
		break;
	case BLOCKMAN::TILE_ENTITY_DROPPER:
		pTileEntity = LordNew TileEntityDropper();
		break;
	case BLOCKMAN::TILE_ENTITY_SIGN:
		pTileEntity = LordNew TileEntitySign();
		break;
	case BLOCKMAN::TILE_ENTITY_MOBSWPAWER:
		pTileEntity = LordNew TileEntityMobSpawner();
		break;
	case BLOCKMAN::TILE_ENTITY_MUSIC:
		pTileEntity = LordNew TileEntityNote();
		break;
	case BLOCKMAN::TILE_ENTITY_PISTON:
		pTileEntity = LordNew TileEntityPiston();
		break;
	case BLOCKMAN::TILE_ENTITY_CAULDRON:
		pTileEntity = LordNew TileEntityBrewingStand();
		break;
	case BLOCKMAN::TILE_ENTITY_ENCHANTTABLE:
		pTileEntity = LordNew TileEntityEnchantmentTable();
		break;
	case BLOCKMAN::TILE_ENTITY_AIRPORTAL:
		pTileEntity = LordNew TileEntityEndPortal();
		break;
	case BLOCKMAN::TILE_ENTITY_CONTROL:
		pTileEntity = LordNew TileEntityCommandBlock();
		break;
	case BLOCKMAN::TILE_ENTITY_BEACON:
		pTileEntity = LordNew TileEntityBeacon();
		break;
	case BLOCKMAN::TILE_ENTITY_SKULL:
		pTileEntity = LordNew TileEntitySkull();
		break;
	case BLOCKMAN::TILE_ENTITY_DLDETECTOR:
		pTileEntity = LordNew TileEntityDaylightDetector();
		break;
	case BLOCKMAN::TILE_ENTITY_HOPPER:
		pTileEntity = LordNew TileEntityHopper();
		break;
	case BLOCKMAN::TILE_ENTITY_COMPARATOR:
		pTileEntity = LordNew TileEntityComparator();
		break;
	default:
		break;
	}

	if (pTileEntity != NULL)
	{
		pTileEntity->readFromNBT(pNBT);
	}

	return pTileEntity;
}

int TileEntity::getBlockMetadata()
{
	if (blockMetadata == -1 && worldObj != NULL)
	{
		blockMetadata = worldObj->getBlockMeta(m_pos);
	}

	return blockMetadata;
}

void TileEntity::onInventoryChanged()
{
	if (worldObj == NULL)
		return;

	blockMetadata = worldObj->getBlockMeta(m_pos);
	worldObj->updateTileEntityChunkAndDoNothing(m_pos, this);

	if (getBlock() != NULL)
	{
		// worldObj->func_96440_m(xCoord, yCoord, zCoord, getBlockType()->getBlockID());
	}

}

float TileEntity::getDistanceFrom(const Vector3& vec)
{
	Vector3 dis = Vector3(m_pos) + Vector3(0.5f, 0.5f, 0.5f) - vec;
	return dis.lenSqr();
}

Block* TileEntity::getBlock()
{
	if (blockType == NULL)
	{
		blockType = BlockManager::sBlocks[worldObj->getBlockId(m_pos)];
	}

	return blockType;
}

}