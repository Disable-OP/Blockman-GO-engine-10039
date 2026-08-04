#include "ChunkClient.h"
#include "cWorld/SectionClient.h"

/** header files from common. */
#include "World/Section.h"
#include "World/World.h"
#include "World/NibbleArray.h"
#include "World/World.h"
#include "World/WorldProvider.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Render/VisualGraph.h"

namespace BLOCKMAN
{

void ChunkClient::initialize2()
{
	m_isSkyLightingUpdated = false;
	m_isBlockLightingUpdated = false;
	m_heightMapMinimum = 0;
	m_isReBuildVisibility = false;
	
	m_updateSkylightColumns = (i8*)LordMalloc(sizeof(i8)* 256);
	memset(m_updateSkylightColumns, 0, sizeof(i8)* 256);
	m_queuedLightChecks = 4096;
	m_updateCD = 520;
}

ChunkClient::ChunkClient(World* pWorld, int x, int z)
	: Chunk(pWorld, x, z)
{
	initialize2();
}

ChunkClient::~ChunkClient()
{
	//LordLogInfo("ChunkClient destruct, pos=(%d,%d)", m_posX, m_posZ);
	LordSafeFree(m_updateSkylightColumns);
}

Section* ChunkClient::CreateSection(int x, int z, int yBase)
{
	return LordNew SectionClient(yBase);
}

SetVisibility* ChunkClient::getSetVisibility(int idx)
{
	if (idx < 0 || idx >= 16)
		return NULL;
	if (!m_pSections)
		return NULL;
	if (!m_pSections[idx])
	{
		return SetVisibility::s_Empty;
	}
	return m_pSections[idx]->getVisibility();
}

bool ChunkClient::setBlocks(const BlockModifys& modifys)
{
	bool ret = Chunk::setBlocks(modifys);
	
	return ret;
}

void ChunkClient::generateSkylightMap()
{
	int maxY = getTopFilledSegment();
	m_heightMapMinimum = Math::MAX_INT;
	int x;
	int z;

	generateHeightMap();

	//update m_heightMapMinimum
	for (x = 0; x < 16; ++x)
	{
		for (z = 0; z < 16; ++z)
		{
			int height = m_heightMap[z << 4 | x];

			if (height < m_heightMapMinimum)
			{
				m_heightMapMinimum = height;
			}
		}
	}

	//light
	for (x = 0; x < 16; ++x)
	{
		for (z = 0; z < 16; ++z)
		{
			if (!m_pWorld->m_provider->hasNoSky)
			{
				int light = 15;				

				for(int y = maxY + 16 - 1; y > 0; --y)
				{
					light -= getBlockLightOpacity(BlockPos(x, y, z));

					if (light > 0)
					{
						Section* pSection = m_pSections[y >> 4];

						if (pSection != NULL)
						{
							pSection->setExtSkylightValue(BlockPos(x, y & 15, z), light);
							m_pWorld->markBlockForRenderUpdate(BlockPos((m_posX << 4) + x, y, (m_posZ << 4) + z));
						}
					}
					else
					{
						break;
					}
					
				}
			}
		}
	}

	for (x = 0; x < 16; ++x)
	{
		for (z = 0; z < 16; ++z)
		{
			propagateSkylightOcclusion(x, z);
		}
	}
}

void ChunkClient::relightBlockLight(const BlockPos& pos, int blockID, int height)
{
	int* lightOpa = BlockManager::sLightOpacity;
	if (lightOpa[blockID & 4095] > 0)
	{
		if (pos.y >= height)
		{
			relightBlock(pos.getPosY());
		}
	}
	else if (pos.y == height - 1)
	{
		relightBlock(pos);
	}

	propagateSkylightOcclusion(pos.x, pos.z);
}

void ChunkClient::propagateSkylightOcclusion(int x, int z)
{
	m_updateSkylightColumns[x + z * 16] = true;
	m_isSkyLightingUpdated = false;
	m_isBlockLightingUpdated = false;
}

void ChunkClient::updateSkylight_do()
{
	//this.worldObj.theProfiler.startSection("recheckGaps");

	if (m_pWorld->doChunksNearChunkExist(BlockPos(m_posX * 16 + 8, 0, m_posZ * 16 + 8), 16))
	{
		for (int x = 0; x < 16; ++x)
		{
			for (int z = 0; z < 16; ++z)
			{
				if (m_updateSkylightColumns[x + z * 16])
				{
					m_updateSkylightColumns[x + z * 16] = false;
					int iy = getHeightValue(x, z);
					int ix = m_posX * 16 + x;
					int iz = m_posZ * 16 + z;
					int iyW = m_pWorld->getChunkHeightMapMinimum(ix - 1, iz);
					int iyE = m_pWorld->getChunkHeightMapMinimum(ix + 1, iz);
					int iyN = m_pWorld->getChunkHeightMapMinimum(ix, iz - 1);
					int iyS = m_pWorld->getChunkHeightMapMinimum(ix, iz + 1);
					int minY = iyW;

					if (iyE < iyW)
						minY = iyE;
					if (iyN < iyW)
						minY = iyN;
					if (iyS < iyW)
						minY = iyS;

					checkSkylightNeighborHeight(ix, iz, minY);
					checkSkylightNeighborHeight(ix - 1, iz, iy);
					checkSkylightNeighborHeight(ix + 1, iz, iy);
					checkSkylightNeighborHeight(ix, iz - 1, iy);
					checkSkylightNeighborHeight(ix, iz + 1, iy);
				}
			}
		}

		m_isSkyLightingUpdated = true;
	}

	//this.worldObj.theProfiler.endSection();
}

void ChunkClient::checkSkylightNeighborHeight(int x, int z, int maxVal)
{
	int h = m_pWorld->getHeightValue(x, z);

	if (h > maxVal)
	{
		if(h-maxVal < 80) // for save cpu time!
			updateSkylightNeighborHeight(x, z, maxVal, h + 1);
	}
	else if (h < maxVal)
	{
		updateSkylightNeighborHeight(x, z, h, maxVal + 1);
	}
}

void ChunkClient::updateSkylightNeighborHeight(int x, int z, int startY, int endY)
{
	if (endY > startY && m_pWorld->doChunksNearChunkExist(BlockPos(x, 0, z), 16))
	{
		for (int y = startY; y < endY; ++y)
		{
			m_pWorld->updateLightByType(SKY_LIGHT_VALUE, BlockPos(x, y, z));
		}
	}
}

bool ChunkClient::checkNeedRebuildAll()
{
	bool ret = m_needRebuildAll;
	if (m_needRebuildAll)
	{
		m_isSkyLightingUpdated = false;
		m_isBlockLightingUpdated = false;
		m_isReBuildVisibility = false;
		m_isPrepared = false;
	}

	return ret;
}

void ChunkClient::relightBlock(const BlockPos& pos)
{
	int x = pos.x;	
	int z = pos.z;
	int preHeight = m_heightMap[z << 4 | x] & 255;
	int curHeight = updateHeightMap(pos);

	if (curHeight != preHeight)
	{
		m_pWorld->markBlocksDirtyVertical(x + m_posX * 16, z + m_posZ * 16, curHeight, preHeight);				
						
		if (!m_pWorld->m_provider->hasNoSky)
		{
			Section* pSection = NULL;
			int startY = Math::Min(curHeight, preHeight);
			int endY = Math::Max(curHeight, preHeight);

			for (int y = startY; y < endY; ++y)
			{
				pSection = m_pSections[y >> 4];

				if (pSection != NULL)
				{
					pSection->setExtSkylightValue(BlockPos(x, y & 15, z), 15);
					m_pWorld->markBlockForRenderUpdate(BlockPos((m_posX << 4) + x, y, (m_posZ << 4) + z));
				}
			}

			
			int light = 15;

			while (curHeight > 0 && light > 0)
			{
				--curHeight;
				int blockOpacity = getBlockLightOpacity(BlockPos(x, curHeight, z));

				if (blockOpacity == 0)
				{
					blockOpacity = 1;
				}

				light -= blockOpacity;

				if (light < 0)
				{
					light = 0;
				}

				pSection = m_pSections[curHeight >> 4];

				if (pSection != NULL)
				{
					pSection->setExtSkylightValue(BlockPos(x, curHeight & 15, z), light);
				}
			}
		}
						
		if (curHeight < m_heightMapMinimum)
		{
			m_heightMapMinimum = curHeight;
		}

		int min_y = Math::Min(preHeight, curHeight);
		int max_y = Math::Max(preHeight, curHeight);
		int ix = m_posX * 16 + x;
		int iz = m_posZ * 16 + z;
		if (!m_pWorld->m_provider->hasNoSky)
		{
			updateSkylightNeighborHeight(ix - 1, iz, min_y, max_y);
			updateSkylightNeighborHeight(ix + 1, iz, min_y, max_y);
			updateSkylightNeighborHeight(ix, iz - 1, min_y, max_y);
			updateSkylightNeighborHeight(ix, iz + 1, min_y, max_y);
			updateSkylightNeighborHeight(ix, iz, min_y, max_y);
		}
	}
}

int ChunkClient::getSavedLightValue(int light, const BlockPos& pos)
{
	Section* pSection = m_pSections[pos.y >> 4];

	if (pSection == NULL)
	{
		return canBlockSeeTheSky(pos) ? light : 0;
	}
	else
	{
		if (light == SKY_LIGHT_VALUE)
		{
			return m_pWorld->m_provider->hasNoSky ? 0 : pSection->getExtSkylightValue(pos.getSection());
		}
		else if (light == BLOCK_LIGHT_VALUE)
		{
			return pSection->getExtBlocklightValue(pos.getSection());
		}
		return light;
	}
}

void ChunkClient::setLightValue(int lightValue, const BlockPos& pos, int l)
{
	Section* pSection = m_pSections[pos.y >> 4];

	if (pSection == NULL)
	{
		pSection = m_pSections[pos.y >> 4] = CreateSection(pos.x >> 4, pos.z >> 4, pos.y >> 4);
		generateSkylightMap();
	}

	if (lightValue == SKY_LIGHT_VALUE)
	{
		if (!m_pWorld->m_provider->hasNoSky)
		{
			pSection->setExtSkylightValue(pos.getSection(), l);
		}
	}
	else if (lightValue == BLOCK_LIGHT_VALUE)
	{
		pSection->setExtBlocklightValue(pos.getSection(), l);
	}
}
//
//bool ChunkClient::setBlocks(const BlockModifys& modifys)
//{
//	bool ret = Chunk::setBlocks(modifys);
//
//	// need notify all the section to rebuild the meshs.
//
//
//}

int ChunkClient::getBlockLightValue(const BlockPos& pos, int l)
{
	Section* pSection = m_pSections[pos.y >> 4];

	if (pSection == NULL)
	{
		return !m_pWorld->m_provider->hasNoSky && l < SKY_LIGHT_VALUE ? SKY_LIGHT_VALUE - l : 0;
	}
	else
	{
		int skylight = m_pWorld->m_provider->hasNoSky ? 0 : pSection->getExtSkylightValue(pos.getSection());

		skylight -= l;
		int blocklight = pSection->getExtBlocklightValue(pos.getSection());

		if (blocklight > skylight)
		{
			skylight = blocklight;
		}

		return skylight;
	}
}

void ChunkClient::updateSkylight()
{
	if (!m_isSkyLightingUpdated && !m_pWorld->m_provider->hasNoSky)
	{
		updateSkylight_do();
	}
}

void ChunkClient::updateBlocklight()
{
	if (m_isBlockLightingUpdated)
		return;

	if (m_pWorld->doChunksNearChunkExist(BlockPos(m_posX * 16 + 8, 0, m_posZ * 16 + 8), 16))
	{
		for (int x = 0; x < 16; ++x)
		{
			for (int z = 0; z < 16; ++z)
			{
				int iy = getHeightValue(x, z);

				for (int y = iy; y >= 0; --y)
				{
					BlockPos pos(x, y, z);
					int blockID = getBlockID(pos);
					int blockLight = BlockManager::sLightValue[blockID];
					/*if (blockLight == 0 && (x != 4 || x != 11 || y !=4 || y != 11 || z != 4 || z != 11))
					continue;*/

					// if update the blockLight error return.
					if (!m_pWorld->updateLightByType(BLOCK_LIGHT_VALUE, BlockPos(m_posX * 16 + pos.x, y, m_posZ * 16 + pos.z)))
						return;
				}
			}
		}

		m_isBlockLightingUpdated = true;
	}
}

void ChunkClient::reBuildVisibility(int idx)
{
	Section* pSection = m_pSections[idx];
	if (!pSection)
		return;
	pSection->reBuildVisibility();
}

void ChunkClient::reBuildVisibility()
{
	if (m_isReBuildVisibility && m_isPrepared)
		return;

	// for every section, rebuild the visualGraph
	for (int i = 0; i < 16; ++i)
	{
		Section* pSection = m_pSections[i];
		if (!pSection)
			continue;
		pSection->reBuildVisibility();
	}
	m_isReBuildVisibility = true;
	m_isPrepared = true;
}

void ChunkClient::enqueueRelightChecks()
{
	for (int i = 0; i < 8; ++i)
	{
		if (m_queuedLightChecks >= 4096)
		{
			return;
		}

		int chunkY = m_queuedLightChecks % 16;
		int chunkX = m_queuedLightChecks / 16 % 16;
		int chunkZ = m_queuedLightChecks / 256;
		++m_queuedLightChecks;
		int x = (m_posX << 4) + chunkX;
		int z = (m_posZ << 4) + chunkZ;
		int* lightValues = BlockManager::sLightValue;

		for (int iy = 0; iy < 16; ++iy)
		{
			int y = (chunkY << 4) + iy;
			BlockPos pos(x, y, z);

			if (m_pSections[chunkY] == NULL &&
				(iy == 0 || iy == 15 || chunkX == 0 || chunkX == 15 || chunkZ == 0 || chunkZ == 15) ||
				m_pSections[chunkY] != NULL &&
				m_pWorld->getBlockId(pos) == 0)
			{
				if (lightValues[m_pWorld->getBlockId(BlockPos(x, y - 1, z))] > 0)
				{
					m_pWorld->updateAllLightTypes(BlockPos(x, y - 1, z));
				}

				if (lightValues[m_pWorld->getBlockId(BlockPos(x, y + 1, z))] > 0)
				{
					m_pWorld->updateAllLightTypes(BlockPos(x, y + 1, z));
				}

				if (lightValues[m_pWorld->getBlockId(BlockPos(x - 1, y, z))] > 0)
				{
					m_pWorld->updateAllLightTypes(BlockPos(x - 1, y, z));
				}

				if (lightValues[m_pWorld->getBlockId(BlockPos(x + 1, y, z))] > 0)
				{
					m_pWorld->updateAllLightTypes(BlockPos(x + 1, y, z));
				}

				if (lightValues[m_pWorld->getBlockId(BlockPos(x, y, z - 1))] > 0)
				{
					m_pWorld->updateAllLightTypes(BlockPos(x, y, z - 1));
				}

				if (lightValues[m_pWorld->getBlockId(BlockPos(x, y, z + 1))] > 0)
				{
					m_pWorld->updateAllLightTypes(BlockPos(x, y, z + 1));
				}

				m_pWorld->updateAllLightTypes(BlockPos(x, y, z));
			}
		}
	}
}

}
