#include "VisualGraph.h"
#include "RenderGlobal.h"

#include "cWorld/Blockman.h"

#include "Tessolator/LineRenderable.h"

namespace BLOCKMAN
{

SetVisibility* SetVisibility::s_Empty = NULL;
SetVisibility* SetVisibility::s_Full = NULL;

void SetVisibility::initialize()
{
	s_Empty = new SetVisibility();
	s_Empty->setAll(true);
	s_Full = new SetVisibility();
	s_Full->setAll(false);
}

void SetVisibility::unInitialize()
{
	LordSafeDelete(s_Empty);
	LordSafeDelete(s_Full);
}

SetVisibility::SetVisibility()
{
	m_visFlag.reset();
}

void SetVisibility::buildVisibility(const FacingArr& arr)
{
	for (FacingArr::const_iterator it = arr.begin(); it != arr.end(); ++it)
	{
		for (FacingArr::const_iterator jt = arr.begin(); jt != arr.end(); ++jt)
			setVisibility(*it, *jt, true);
	}
}

void SetVisibility::setVisibility(Facing* facing1, Facing* facing2, bool bVisible)
{
	if (bVisible)
	{
		m_visFlag.set(facing1->getIndex() * s_FaceNum + facing2->getIndex());
		m_visFlag.set(facing2->getIndex() * s_FaceNum + facing1->getIndex());
	}
	else
	{
		m_visFlag.reset(facing1->getIndex() * s_FaceNum + facing2->getIndex());
		m_visFlag.reset(facing2->getIndex() * s_FaceNum + facing1->getIndex());
	}
}

void SetVisibility::setAll(bool bVisible)
{
	if (bVisible)
		m_visFlag.set();
	else
		m_visFlag.reset();
}

bool SetVisibility::getVisibility(Facing* facing1, Facing* facing2)
{
	return m_visFlag[facing1->getIndex() + facing2->getIndex() * s_FaceNum];
}

void SetVisibility::outputInfo()
{
	// output 15 bit 
	String ret;
	int c = 1;
	for (int i = 0; i < 6; ++i)
	{
		for (int j = i + 1; j < 6; ++j)
		{
			ret += StringUtil::Format("[%d](%08X) {%d}, ", c, &m_visFlag, m_visFlag[i * 6 + j] ? 1 : 0);
			++c;
		}
	}
	LordLogInfo("%s", ret.c_str());
}

void SetVisibility::buildVisualHelp(const BlockPos& pos)
{
	Box aabb(Vector3(pos), Vector3(pos) + Vector3(16.f, 16.f, 16.f));
	ui32 color = 0xFF777777;
	
	RenderGlobal* global = Blockman::Instance()->m_globalRender;
	LineVertexFmt* points = NULL;
	LineRenderable* renderable = global->getLineRenderable(5);
	renderable->getLockPtr_line(5, points);
	points[0].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, color);
	points[1].set(aabb.vMax.x, aabb.vMin.y, aabb.vMin.z, color);
	points[2].set(aabb.vMax.x, aabb.vMin.y, aabb.vMax.z, color);
	points[3].set(aabb.vMin.x, aabb.vMin.y, aabb.vMax.z, color);
	points[4].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, color);

	renderable = global->getLineRenderable(5);
	renderable->getLockPtr_line(5, points);
	points[0].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, color);
	points[1].set(aabb.vMax.x, aabb.vMax.y, aabb.vMin.z, color);
	points[2].set(aabb.vMax.x, aabb.vMax.y, aabb.vMax.z, color);
	points[3].set(aabb.vMin.x, aabb.vMax.y, aabb.vMax.z, color);
	points[4].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, color);

	renderable = global->getLineRenderable(8);
	renderable->getLockPtr_seg(8, points);
	points[0].set(aabb.vMin.x, aabb.vMin.y, aabb.vMin.z, color);
	points[1].set(aabb.vMin.x, aabb.vMax.y, aabb.vMin.z, color);
	points[2].set(aabb.vMax.x, aabb.vMin.y, aabb.vMin.z, color);
	points[3].set(aabb.vMax.x, aabb.vMax.y, aabb.vMin.z, color);
	points[4].set(aabb.vMax.x, aabb.vMin.y, aabb.vMax.z, color);
	points[5].set(aabb.vMax.x, aabb.vMax.y, aabb.vMax.z, color);
	points[6].set(aabb.vMin.x, aabb.vMin.y, aabb.vMax.z, color);
	points[7].set(aabb.vMin.x, aabb.vMax.y, aabb.vMax.z, color);
	
	// construct 15 bit visibility visual helps.
	Vector3 center = Vector3(pos) + Vector3(8.f, 8.f, 8.f);
	Vector3 dir[6];
	for (int i = 0; i < 6; ++i)
		dir[i] = center + Vector3(offsetForSide[i]) * 8.f;
	
	renderable = global->getLineRenderable(30);
	renderable->getLockPtr_seg(30, points);
	int pidx = 0;
	for (int i = 0; i < 6; ++i)
	{
		for (int j = i + 1; j < 6; ++j)
		{
			ui32 color = m_visFlag[i * 6 + j] ? 0xFF99FF99 : 0xFF9999FF;
			points[pidx++].set(dir[i].x, dir[i].y, dir[i].z, color);
			points[pidx++].set(dir[j].x, dir[j].y, dir[j].z, color);
		}
	}
	LordAssert(pidx == 30);
}

int* VisualGraph::INDEX_OF_EDGES = NULL;
int VisualGraph::INDEX_OF_EDGES_LEN = 1352; // 1024 + 256 + 64 + 8.

VisualGraph::VisualGraph()
	: m_emptyQuota(4096)
{
	m_visibleFlag.reset();
}

void VisualGraph::setOpaque(const BlockPos& pos)
{
	m_visibleFlag.set(getBlockIdx(pos));
	--m_emptyQuota;
}

void VisualGraph::computeVisibility(SetVisibility* setVisibility)
{
	if (4096 - m_emptyQuota < 256)
	{
		setVisibility->setAll(true);
	}
	else if (m_emptyQuota == 0)
	{
		setVisibility->setAll(false);
	}
	else
	{
		setVisibility->setAll(false);
		for (int i = 0; i < INDEX_OF_EDGES_LEN; ++i)
		{
			int edge = INDEX_OF_EDGES[i];

			if (!m_visibleFlag[edge]) // is not opaque
			{
				setVisibility->buildVisibility(floodFill(edge));
			}
		}
	}
}

FacingArr VisualGraph::floodFill(int pakPos)
{
	FacingArr result;
	deque<int>::type ringQueue;
	ringQueue.push_back(pakPos);
	m_visibleFlag.set(pakPos);

	while (!ringQueue.empty())
	{
		int pos = ringQueue.front();
		ringQueue.pop_front();
		addEdges(pos, result);

		for (int i = 0; i < 6; ++i)
		{
			Facing* face = Facing::VALUES[i];
			int neighborPos = getNeighborIndexAtFace(pos, face);

			if (neighborPos >= 0 && !m_visibleFlag[neighborPos])
			{
				m_visibleFlag.set(neighborPos);
				ringQueue.push_back(neighborPos);
			}
		}
	}

	return result;
}

void VisualGraph::addEdges(int edges, FacingArr& facingArr)
{
	int x = edges >> 0 & 15;

	if (x == 0)
	{
		facingArr.insert(Facing::WEST);
	}
	else if (x == 15)
	{
		facingArr.insert(Facing::EAST);
	}

	int y = edges >> 8 & 15;

	if (y == 0)
	{
		facingArr.insert(Facing::DOWN);
	}
	else if (y == 15)
	{
		facingArr.insert(Facing::UP);
	}

	int z = edges >> 4 & 15;

	if (z == 0)
	{
		facingArr.insert(Facing::NORTH);
	}
	else if (z == 15)
	{
		facingArr.insert(Facing::SOUTH);
	}
}

int VisualGraph::getNeighborIndexAtFace(int blockPosPak, Facing* facing)
{
	switch (facing->getIndex())
	{
	case 0:
		if ((blockPosPak >> 8 & 15) == 0)
			return -1;

		return blockPosPak - s_Ybegin;

	case 1:
		if ((blockPosPak >> 8 & 15) == 15)
			return -1;

		return blockPosPak + s_Ybegin;

	case 2:
		if ((blockPosPak >> 4 & 15) == 0)
			return -1;

		return blockPosPak - s_Zbegin;

	case 3:
		if ((blockPosPak >> 4 & 15) == 15)
			return -1;

		return blockPosPak + s_Zbegin;

	case 4:
		if ((blockPosPak >> 0 & 15) == 0)
			return -1;

		return blockPosPak - s_Xbegin;

	case 5:
		if ((blockPosPak >> 0 & 15) == 15)
			return -1;

		return blockPosPak + s_Xbegin;

	default:
		return -1;
	}
}

void VisualGraph::initialize()
{
	LordAssert(!INDEX_OF_EDGES);
	INDEX_OF_EDGES = (int*)LordMalloc(sizeof(int) * INDEX_OF_EDGES_LEN);
	
	int var2 = 0;

	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; ++z)
			{
				if (x == 0 || x == 15 || y == 0 || y == 15 || z == 0 || z == 15)
				{
					INDEX_OF_EDGES[var2++] = getBlockIdx(BlockPos(x, y, z));
				}
			}
		}
	}
}

void VisualGraph::unInitialize()
{
	LordAssert(INDEX_OF_EDGES);
	LordSafeFree(INDEX_OF_EDGES);
}

}