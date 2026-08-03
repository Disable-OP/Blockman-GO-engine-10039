#include "Section.h"
#include "NibbleArray.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Block/BlockUtil.h"

namespace BLOCKMAN
{

const int Section::BLOCK_ARRAY_SIZE;

Section::Section(int yBase)
	: m_yBase(yBase)
	, m_blockRefCount(0)
	, m_tickRefCount(0)
	, m_customBlockRefCount(0)
	, m_blockMetadataArray(BLOCK_ARRAY_SIZE, 4)
{
}

int Section::getBlockID(const BlockPos& pos)
{
	int leastSignificant = m_blockLSBArray[pos.y << 8 | pos.z << 4 | pos.x] & 255;
	if (!m_blockHSBArray)
	{
		return leastSignificant;
	}
	return ((m_blockHSBArray->get(pos) & 15) << 8) | leastSignificant;
}

void Section::setBlockID(const BlockPos& pos, int blockID)
{
	int oldID = getBlockID(pos);
	
	Block** pBlockArr = BlockManager::sBlocks;
	LordAssert(pBlockArr);

	if (oldID == 0 && blockID != 0)
	{
		++m_blockRefCount;

		if (pBlockArr[blockID] != NULL &&
			pBlockArr[blockID]->getTickRandomly())
		{
			++m_tickRefCount;
		}
	}
	else if (oldID != 0 && blockID == 0)
	{
		--m_blockRefCount;

		if (pBlockArr[oldID] != NULL &&
			pBlockArr[oldID]->getTickRandomly())
		{
			--m_tickRefCount;
		}
	}
	else if (pBlockArr[oldID] != NULL &&
		pBlockArr[oldID]->getTickRandomly() && 
		(pBlockArr[blockID] == NULL ||
			!pBlockArr[blockID]->getTickRandomly()))
	{
		--m_tickRefCount;
	}
	else if ((pBlockArr[oldID] == NULL || 
		!pBlockArr[oldID]->getTickRandomly()) &&
		pBlockArr[blockID] != NULL &&
		pBlockArr[blockID]->getTickRandomly())
	{
		++m_tickRefCount;
	}

	if (BlockUtil::isBasicBlock(oldID) && BlockUtil::isECBlock(blockID))
	{
		++m_customBlockRefCount;
		if (!m_blockHSBArray)
		{
			m_blockHSBArray = LORD::make_unique<NibbleArray>(BLOCK_ARRAY_SIZE, 4);
		}
	}
	else if (BlockUtil::isECBlock(oldID) && BlockUtil::isBasicBlock(blockID))
	{
		--m_customBlockRefCount;
		if (m_customBlockRefCount == 0)
		{
			m_blockHSBArray = nullptr;
		}
	}

	LordAssert(blockID >= 0);
	m_blockLSBArray[pos.y << 8 | pos.z << 4 | pos.x] = blockID & 255;
	if (m_blockHSBArray)
	{
		m_blockHSBArray->set(pos, blockID >> 8 & 15);
	}
}

int Section::getBlockMeta(const BlockPos& pos)
{
	return m_blockMetadataArray.get(pos);
}

void Section::setBlockMeta(const BlockPos& pos, int metadata)
{
	m_blockMetadataArray.set(pos, metadata);
}

bool Section::isEmpty()
{
	return m_blockRefCount == 0;
}

bool Section::getNeedsRandomTick()
{
	return m_tickRefCount > 0;
}

void Section::removeInvalidBlocks()
{
	m_blockRefCount = 0;
	m_tickRefCount = 0;
	m_customBlockRefCount = 0;

	Block** pBlockArr = BlockManager::sBlocks;
	LordAssert(pBlockArr);

	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; ++z)
			{
				BlockPos pos(x, y, z);
				int blockID = getBlockID(pos);

				if (blockID > 0)
				{
					if (!pBlockArr[blockID])
					{
						m_blockLSBArray[y<<8 | z<<4 | x] = 0;
						if (m_blockHSBArray)
						{
							m_blockHSBArray->set(pos, 0);
						}
					}
					else
					{
						++m_blockRefCount;

						if (BlockUtil::isECBlock(blockID))
						{
							++m_customBlockRefCount;
						}

						if (pBlockArr[blockID]->getTickRandomly())
						{
							++m_tickRefCount;
						}
					}
				}
			}
		}
	}

	if (m_customBlockRefCount == 0)
	{
		m_blockHSBArray = nullptr;
	}
}

i8* Section::getBlockLSBArray()
{
	return m_blockLSBArray;
}

NibbleArray* Section::getMetadataArray()
{
	return &m_blockMetadataArray;
}

void Section::generateBlockHSBArrayNbt(const String & tagName, NbtGenerator & generator) const
{
	if (!m_blockHSBArray)
	{
		return;
	}
	generator.writeTagByteArrayStart(tagName, m_blockHSBArray->size());
	for (auto byte : *m_blockHSBArray)
	{
		generator.writeTagByte(byte);
	}
}

void Section::parseBlockHSBArrayNbt(const NbtTagEvent & event, NbtParser & parser)
{
	m_blockHSBArray = LORD::make_unique<NibbleArray>(BLOCK_ARRAY_SIZE, 4);
	parser.nextEvent();
	for (size_t i = 0;
		!parser.currentEvent().isListEnd() && i < m_blockHSBArray->size();
		parser.nextEvent(), ++i)
	{
		(*m_blockHSBArray)[i] = parser.currentEvent().byteValue;
	}
}

}