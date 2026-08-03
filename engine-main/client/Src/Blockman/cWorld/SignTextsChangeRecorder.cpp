#include "SignTextsChangeRecorder.h"
#include "Chunk/Chunk.h"
#include "World/World.h"
#include "TileEntity/TileEntitys.h"

namespace BLOCKMAN
{
	void SignTextsChangeRecorder::applyChanges(const ChunkPtr& chunk) const
	{
		LordAssert(chunk);
		if (chunk->isNonexistent())
		{
			return;
		}
		for (const auto& record : m_changeRecords)
		{
			const auto& position = record.first;
			const auto& texts = record.second;
			if (chunk->m_posX != position.x >> 4 || chunk->m_posZ != position.z >> 4)
			{
				continue;
			}
			LordAssert(chunk->m_pWorld);
			auto signTileEntity = dynamic_cast<TileEntitySign*>(chunk->m_pWorld->getBlockTileEntity(position));
			if (!signTileEntity)
			{
				LordLogError("Changes of sign texts is buffered but there's no TileEntitySign at (%d, %d, %d)", position.x, position.y, position.z);
				continue;
			}
			for (size_t i = 0; i < texts.size(); ++i)
			{
				signTileEntity->setText(i, texts[i]);
			}
		}
	}
}
