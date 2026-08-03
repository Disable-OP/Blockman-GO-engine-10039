#include "CustomBlock.h"
#include "World/World.h"
#include "Trigger/TriggerManagers.h"

namespace BLOCKMAN
{
	vector<CustomBlock*>::type CustomBlock::customBlockList;

	CustomBlock::CustomBlock(int id, const String &nameSpace, const String &name, const BlockConfig & config)
		: Block(
			id, 
			BM_Material(
				-1, 
				config.flammable, 
				config.state == BlockState::GAS,
				config.transparent || config.state == BlockState::GAS, 
				/* TODO: about digging */ true, 
				config.state == BlockState::GAS ? 1 : 0,
				true, 
				config.state == BlockState::SOLID, 
				false, 
				config.state == BlockState::SOLID, 
				config.state == BlockState::SOLID /* TODO: I'm not sure what's the effect of this field */),
			name, 
			!config.transparent && config.state == BlockState::SOLID)
		, m_namespace(nameSpace)
		, m_state(config.state)
		, m_collisionBox(config.collisionBox)
	{
		setResistance(static_cast<float>(config.resistance) / 3);
		TRIGGER_MANAGER::registerTriggers(getBlockID(), config.triggers);
		m_clickable = TriggerClickManager::Instance()->blockHasTrigger(getBlockID());
	}

	CustomBlock::~CustomBlock()
	{
		TRIGGER_MANAGER::unregisterTriggers(getBlockID());
	}

	int CustomBlock::idDropped(int meta, Random & rand, int fortune)
	{
		if (m_state == BlockState::GAS)
		{
			return 0;
		}
		else
		{
			return Block::getItemBlockId();
		}
	}

	int CustomBlock::quantityDropped(Random & rand)
	{
		if (m_state == BlockState::GAS)
		{
			return 0;
		}
		else
		{
			return Block::quantityDropped(rand);
		}
	}

	Box CustomBlock::getCollisionBoundingBox(World * pWorld, const BlockPos & pos)
	{
		if (m_state == BlockState::GAS)
		{
			return Box();
		}
		else
		{
			return {m_collisionBox.vMin + pos, m_collisionBox.vMax + pos};
		}
	}

	bool CustomBlock::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
	{
		Block::onBlockActivated(pWorld, pos, pPlayer, face, hit);
		TriggerClickManager::Instance()->checkTriggers(pWorld, pos, pPlayer);
		return m_clickable;
	}

	bool CustomBlock::renderAsNormalBlock() const
	{
		return m_state == BlockState::SOLID;
	}

	bool CustomBlock::isCollidable() const 
	{ 
		return m_state == BlockState::SOLID; 
	}
}
