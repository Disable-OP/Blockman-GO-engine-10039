#include "SectionClient.h"

/** header files from common. */
#include "World/Section.h"
#include "World/NibbleArray.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Render/VisualGraph.h"

namespace BLOCKMAN
{

SectionClient::SectionClient(int yBase)
	: Section(yBase)
	, m_blocklightArray(NULL)
	, m_skylightArray(NULL)
	, m_isVisSetBuild(false)
{
	m_setVisibility = new SetVisibility();
	m_blocklightArray = LordNew NibbleArray(Section::BLOCK_ARRAY_SIZE, 4);
	m_skylightArray = LordNew NibbleArray(Section::BLOCK_ARRAY_SIZE, 4);
}

SectionClient::~SectionClient()
{
	delete m_setVisibility;
	LordSafeDelete(m_blocklightArray);
	LordSafeDelete(m_skylightArray);
}

void SectionClient::setExtSkylightValue(const BlockPos& pos, int skylightvalue)
{
	LordAssertX(m_skylightArray, "m_skylightArray is NULL!");
	m_skylightArray->set(pos, skylightvalue);
}

int SectionClient::getExtSkylightValue(const BlockPos& pos)
{
	LordAssertX(m_skylightArray, "m_skylightArray is NULL!");
	return m_skylightArray->get(pos);
}

void SectionClient::setExtBlocklightValue(const BlockPos& pos, int blocklightvalue)
{
	LordAssertX(m_blocklightArray, "m_blocklightArray is NULL!");
	m_blocklightArray->set(pos, blocklightvalue);
}

int SectionClient::getExtBlocklightValue(const BlockPos& pos)
{
	LordAssertX(m_blocklightArray, "m_blocklightArray is NULL!");
	return m_blocklightArray->get(pos);
}

void SectionClient::setBlockID(const BlockPos& pos, int id)
{
	m_lock.Lock();
	Section::setBlockID(pos, id);
	m_isVisSetBuild = false;
	m_lock.Unlock();
}

void SectionClient::setBlockMeta(const BlockPos& pos, int meta)
{
	m_lock.Lock();
	Section::setBlockMeta(pos, meta);
	m_isVisSetBuild = false;
	m_lock.Unlock();
}

void SectionClient::reBuildVisibility()
{
	if (m_isVisSetBuild)
		return;

	Block** pBlocks = BlockManager::sBlocks;
	VisualGraph visGraph;
	m_lock.Lock();

	for (int x = 0; x < 16; ++x)
	{
		for (int z = 0; z < 16; ++z)
		{
			for (int y = 0; y < 16; ++y)
			{
				BlockPos pos(x, y, z);
				int blockID = getBlockID(pos);
				if (blockID > 0 && pBlocks[blockID]->isOpaqueCube())
				{
					visGraph.setOpaque(pos);
				}
			}
		}
	}
	m_lock.Unlock();
	visGraph.computeVisibility(m_setVisibility);
	m_isVisSetBuild = true;
}


}