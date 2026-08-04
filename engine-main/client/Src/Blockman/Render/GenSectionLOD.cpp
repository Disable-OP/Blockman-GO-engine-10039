#include "GenSectionLOD.h"
#include "BM_TypeDef.h"
#include "SectionRenderer.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Rasterizer.h"

#include "Block/Blocks.h"
#include "Block/BlockManager.h"

namespace BLOCKMAN
{

GenSectionLod::GenSectionLod(RasterizerMesh* pMesh, const Vector3i& sectionBegin)
	: m_pMesh(pMesh)
	, m_sectionId(sectionBegin)
{}

GenSectionLod::~GenSectionLod()
{}


void GenSectionLod::generate(ui32 rawdata_lod[][20][20])
{
	solidCnt = 0;
	m_pMesh->clean();
	m_pMesh->m_sectionId = m_sectionId;
	const bool useGreedyAlgorithm = true;

	ui8 opaqueFlags[20][20][20];
	memset(opaqueFlags, 0, sizeof(opaqueFlags));
	for (int y = 0; y < 20; ++y)
	{
		for (int x = 0; x < 20; ++x)
		{
			for (int z = 0; z < 20; ++z)
			{
				ui16 blockid = rawdata_lod[y][x][z];
				if (blockid == 0)
					continue;
				Block* pBlock = BlockManager::sBlocks[blockid];
				if (!pBlock)
					continue;
				if (pBlock->isOpaqueCube())
				{
					opaqueFlags[y][x][z] = 1;
					solidCnt++;
				}
			}
		}
	}

	map<Vector2i, Stripe>::type stripeMap;//pos,length
	map<Vector2i, Stripe>::type stripeMap_2;//pos,length
	ui32 itbegin = 2;
	ui32 itend = 18;
	Vector3 offsetMesh(2.f, 2.f, 2.f);

	for (ui32 z = itbegin - 1; z < itend; ++z)
	{
		stripeMap.clear();
		Stripe targetStripe;
		stripeMap_2.clear();
		Stripe targetStripe_2;

		for (ui32 y = itbegin; y < itend; ++y)
		{
			for (ui32 x = itbegin; x < itend; ++x)
			{
				ui8 flag_1 = opaqueFlags[y][x][z];
				ui8 flag_2 = opaqueFlags[y][x][z+1];
				
				bool shouldGenFront = ShouldGenCubeFace(flag_1, flag_2);
				bool shouldGenBack = ShouldGenCubeFace(flag_2, flag_1);

				if (useGreedyAlgorithm)
				{
					if (flag_1 != targetStripe.targetID || shouldGenFront == false)
					{
						if (targetStripe.length != 0 && targetStripe.targetID != 0xFF)
							stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));

						if (shouldGenFront)
						{
							Vector2i pos(x, y);
							targetStripe.Reset(pos, 1, flag_1);
							if (x == itend - 1)
							{
								stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
								targetStripe.Reset();
							}
						}
						else
							targetStripe.Reset();
					}
					else
					{
						targetStripe.length++;
						if (x == itend - 1 && targetStripe.targetID != -1)
						{
							stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
							targetStripe.Reset();
						}
					}

					if (flag_2 != targetStripe_2.targetID || shouldGenBack == false)
					{
						if (targetStripe_2.length != 0 && targetStripe_2.targetID != 0xFF)
							stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));

						if (shouldGenBack)
						{
							Vector2i pos(x, y);
							targetStripe_2.Reset(pos, 1, flag_2);
							if (x == itend - 1)
							{
								stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
								targetStripe_2.Reset();
							}
						}
						else
							targetStripe_2.Reset();
					}
					else
					{
						targetStripe_2.length++;
						if (x == itend - 1 && targetStripe_2.targetID != -1)
						{
							stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
							targetStripe_2.Reset();
						}
					}
				}
				else
				{
					if (shouldGenFront)
					{
						targetStripe.Reset(Vector2i(x, y), 1, flag_1);
						stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
					}

					if (shouldGenBack)
					{
						targetStripe_2.Reset(Vector2i(x, y), 1, flag_2);
						stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
					}
				}
			}
		}

		auto it = stripeMap.begin();
		for (; it != stripeMap.end(); ++it)
		{
			Stripe& curStripe = it->second;
			Vector2i curBias = it->first;//for curbx curby ==> z , y
			int curLen = curStripe.length;
			int curheight = 1;

			if (curLen == 0)
				continue;

			if (useGreedyAlgorithm)
			{
				for (int y = curBias.y + 1; y < static_cast<int>(itend); ++y)
				{
					Vector2i searchBias(curBias.x, y);
					auto it_search = stripeMap.find(searchBias);
					if (it_search != stripeMap.end())
					{
						if (it_search->second.CanMerge(curStripe))
						{
							stripeMap[searchBias].length = 0;
							curheight++;
						}
						else
							break;
					}
					else
						break;
				}
			}
			Vector3 posMin(static_cast<float>(curBias.x), static_cast<float>(curBias.y), static_cast<float>(z + 1));
			Vector3 posMax(static_cast<float>(curBias.x + curLen), static_cast<float>(curBias.y + curheight), static_cast<float>(z + 1));
			AddQuad(curStripe.targetID, posMin - offsetMesh, posMax - offsetMesh, Side_Front, curStripe);
		}

		auto it_2 = stripeMap_2.begin();
		for (; it_2 != stripeMap_2.end(); ++it_2)
		{
			Stripe& curStripe = it_2->second;
			Vector2i curBias = it_2->first;//for xz, y ==> z
			int curLen = curStripe.length;
			int curheight = 1;

			if (curLen == 0)
				continue;

			if (useGreedyAlgorithm)
			{
				for (int y = curBias.y + 1; y < static_cast<int>(itend); ++y)
				{
					Vector2i searchBias(curBias.x, y);
					auto it_search = stripeMap_2.find(searchBias);
					if (it_search != stripeMap_2.end())
					{
						if (it_search->second.CanMerge(curStripe))
						{
							stripeMap_2[searchBias].length = 0;
							curheight++;
						}
						else
							break;
					}
					else
						break;
				}
			}
			Vector3 posMin(static_cast<float>(curBias.x), static_cast<float>(curBias.y), static_cast<float>(z + 1));
			Vector3 posMax(static_cast<float>(curBias.x + curLen), static_cast<float>(curBias.y + curheight), static_cast<float>(z + 1));
			AddQuad(curStripe.targetID, posMin - offsetMesh, posMax - offsetMesh, Side_Back, curStripe);
		}
	}

	for (ui32 x = itbegin - 1; x < itend; ++x)
	{
		stripeMap.clear(); Stripe targetStripe;
		stripeMap_2.clear(); Stripe targetStripe_2;

		for (ui32 y = itbegin; y < itend; ++y)
		{
			for (ui32 z = itbegin; z < itend; ++z)
			{
				ui8 flag_1 = opaqueFlags[y][x][z];
				ui8 flag_2 = opaqueFlags[y][x+1][z];

				bool shouldGenRight = ShouldGenCubeFace(flag_1, flag_2);
				bool shouldGenLeft = ShouldGenCubeFace(flag_2, flag_1);

				if (useGreedyAlgorithm)
				{
					if (flag_1 != targetStripe.targetID || shouldGenRight == false)
					{
						if (targetStripe.length != 0 && targetStripe.targetID != 0xFF)
							stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));

						if (shouldGenRight)
						{
							Vector2i pos(z, y);
							targetStripe.Reset(pos, 1, flag_1);
							if (z == itend - 1)
							{
								stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
								targetStripe.Reset();
							}
						}
						else
							targetStripe.Reset();
					}
					else
					{
						targetStripe.length++;
						if (z == itend - 1 && targetStripe.targetID != -1)
						{
							stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
							targetStripe.Reset();
						}
					}

					if (flag_2 != targetStripe_2.targetID || shouldGenLeft == false)
					{
						if (targetStripe_2.length != 0 && targetStripe_2.targetID != -1)
							stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));

						if (shouldGenLeft)
						{
							Vector2i pos(z, y);
							targetStripe_2.Reset(pos, 1, flag_2);
							if (z == itend - 1)
							{
								stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
								targetStripe_2.Reset();
							}
						}
						else
							targetStripe_2.Reset();
					}
					else
					{
						targetStripe_2.length++;
						if (z == itend - 1 && targetStripe_2.targetID != -1)
						{
							stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
							targetStripe_2.Reset();
						}
					}
				}
				else
				{
					if (shouldGenRight)
					{
						targetStripe.Reset(Vector2i(z, y), 1, flag_1);
						stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
					}
					if (shouldGenLeft)
					{
						targetStripe_2.Reset(Vector2i(z, y), 1, flag_2);
						stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
					}
				}
			}
		}

		auto it = stripeMap.begin();
		for (; it != stripeMap.end(); ++it)
		{
			Stripe& curStripe = it->second;
			Vector2i curBias = it->first;//for curbx curby ==> z , y
			int curLen = curStripe.length;
			int curheight = 1;

			if (curLen == 0)
				continue;

			if (useGreedyAlgorithm)
			{
				for (int y = curBias.y + 1; y < static_cast<int>(itend); ++y)
				{
					Vector2i searchBias(curBias.x, y);
					auto it_search = stripeMap.find(searchBias);
					if (it_search != stripeMap.end())
					{
						if (it_search->second.CanMerge(curStripe))
						{
							stripeMap[searchBias].length = 0;
							curheight++;
						}
						else
							break;
					}
					else
						break;
				}
			}
			Vector3 posMin(static_cast<float>(x + 1), static_cast<float>(curBias.y), static_cast<float>(curBias.x));
			Vector3 posMax(static_cast<float>(x + 1), static_cast<float>(curBias.y + curheight), static_cast<float>(curBias.x + curLen));
			AddQuad(curStripe.targetID, posMin - offsetMesh, posMax - offsetMesh, Side_Right, curStripe);
		}

		auto it_2 = stripeMap_2.begin();
		for (; it_2 != stripeMap_2.end(); ++it_2)
		{
			Stripe& curStripe = it_2->second;
			Vector2i curBias = it_2->first;//for xz, y ==> z
			int curLen = curStripe.length;
			int curheight = 1;

			if (curLen == 0)
				continue;

			if (useGreedyAlgorithm)
			{
				for (int y = curBias.y + 1; y < static_cast<int>(itend); ++y)
				{
					Vector2i searchBias(curBias.x, y);
					auto it_search = stripeMap_2.find(searchBias);
					if (it_search != stripeMap_2.end())
					{
						if (it_search->second.CanMerge(curStripe))
						{
							stripeMap_2[searchBias].length = 0;
							curheight++;
						}
						else
							break;
					}
					else
						break;
				}
			}
			Vector3 posMin(static_cast<float>(x + 1), static_cast<float>(curBias.y), static_cast<float>(curBias.x));
			Vector3 posMax(static_cast<float>(x + 1), static_cast<float>(curBias.y + curheight), static_cast<float>(curBias.x + curLen));
			AddQuad(curStripe.targetID, posMin - offsetMesh, posMax - offsetMesh, Side_Left, curStripe);
		}
	}

	for (ui32 y = itbegin - 1; y < itend; ++y)
	{
		stripeMap.clear(); Stripe targetStripe;
		stripeMap_2.clear(); Stripe targetStripe_2;

		for (ui32 z = itbegin; z < itend; ++z)
		{
			for (ui32 x = itbegin; x < itend; ++x)
			{
				ui8 flag_1 = opaqueFlags[y][x][z];
				ui8 flag_2 = opaqueFlags[y+1][x][z];

				bool shouldGenTop = ShouldGenCubeFace(flag_1, flag_2);
				bool shouldGenBtm = ShouldGenCubeFace(flag_2, flag_1);

				if (useGreedyAlgorithm)
				{
					if (flag_1 != targetStripe.targetID || shouldGenTop == false)
					{
						if (targetStripe.length != 0 && targetStripe.targetID != -1)
							stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));

						if (shouldGenTop)
						{
							Vector2i pos(x, z);
							targetStripe.Reset(pos, 1, flag_1);
							if (x == itend - 1)
							{
								stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
								targetStripe.Reset();
							}
						}
						else
							targetStripe.Reset();
					}
					else
					{
						targetStripe.length++;
						if (x == itend - 1 && targetStripe.targetID != -1)
						{
							stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
							targetStripe.Reset();
						}
					}

					if (flag_2 != targetStripe_2.targetID || shouldGenBtm == false)
					{
						if (targetStripe_2.length != 0 && targetStripe_2.targetID != -1)
							stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));

						if (shouldGenBtm)
						{
							Vector2i pos(x, z);
							targetStripe_2.Reset(pos, 1, flag_2);
							if (x == itend - 1)
							{
								stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
								targetStripe_2.Reset();
							}
						}
						else
							targetStripe_2.Reset();
					}
					else
					{
						targetStripe_2.length++;
						if (x == itend - 1 && targetStripe_2.targetID != -1)
						{
							stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
							targetStripe_2.Reset();
						}
					}
				}
				else
				{
					if (shouldGenTop)
					{
						targetStripe.Reset(Vector2i(x, z), 1, flag_1);
						stripeMap.insert(std::make_pair(targetStripe.bias2D, targetStripe));
					}

					if (shouldGenBtm)
					{
						targetStripe_2.Reset(Vector2i(x, z), 1, flag_2);
						stripeMap_2.insert(std::make_pair(targetStripe_2.bias2D, targetStripe_2));
					}
				}
			}
		}

		auto it = stripeMap.begin();
		for (; it != stripeMap.end(); it++)
		{
			Stripe& curStripe = it->second;
			Vector2i curBias = it->first;//for xz, y ==> z
			int curLen = curStripe.length;
			int curheight = 1;

			if (curLen == 0)
				continue;

			if (useGreedyAlgorithm)
			{
				for (int z = curBias.y + 1; z < static_cast<int>(itend); ++z)
				{
					Vector2i searchBias(curBias.x, z);
					auto it_search = stripeMap.find(searchBias);
					if (it_search != stripeMap.end())
					{
						if (it_search->second.CanMerge(curStripe))
						{
							stripeMap[searchBias].length = 0;
							curheight++;
						}
						else
							break;
					}
					else
						break;
				}
			}
			Vector3 posMin(static_cast<float>(curBias.x), static_cast<float>(y + 1), static_cast<float>(curBias.y));
			Vector3 posMax(static_cast<float>(curBias.x + curLen), static_cast<float>(y + 1), static_cast<float>(curBias.y + curheight));
			AddQuad(curStripe.targetID, posMin - offsetMesh, posMax - offsetMesh, Side_Top, curStripe);
		}

		auto it_2 = stripeMap_2.begin();
		for (; it_2 != stripeMap_2.end(); it_2++)
		{
			Stripe& curStripe = it_2->second;
			Vector2i curBias = it_2->first;//for xz, y ==> z
			int curLen = curStripe.length;
			int curheight = 1;

			if (curLen == 0)
				continue;

			if (useGreedyAlgorithm)
			{
				for (int z = curBias.y + 1; z < static_cast<int>(itend); ++z)
				{
					Vector2i searchBias(curBias.x, z);
					auto it_search = stripeMap_2.find(searchBias);
					if (it_search != stripeMap_2.end())
					{
						if (it_search->second.CanMerge(curStripe))
						{
							stripeMap_2[searchBias].length = 0;
							curheight++;
						}
						else
							break;
					}
					else
						break;
				}
			}
			Vector3 posMin(static_cast<float>(curBias.x), static_cast<float>(y + 1), static_cast<float>(curBias.y));
			Vector3 posMax(static_cast<float>(curBias.x + curLen), static_cast<float>(y + 1), static_cast<float>(curBias.y + curheight));
			AddQuad(curStripe.targetID, posMin - offsetMesh, posMax - offsetMesh, Side_Bottom, curStripe);
		}
	}
}


void GenSectionLod::AddQuad(ui8 opaqueFlag, Vector3 posMin, Vector3 posMax, Side side, Stripe &stripe)
{
	if (opaqueFlag == 0)
		return;

	int indicebuffers[4];
	Vector3 lbPos, ltPos, rtPos, rbPos;

	switch (side)
	{
	case Side_Right:
	{
		indicebuffers[0] = 0; indicebuffers[1] = 3; indicebuffers[2] = 4; indicebuffers[3] = 7;
		lbPos = posMin;
		ltPos = Vector3(posMin.x, posMax.y, posMin.z);
		rtPos = posMax;
		rbPos = Vector3(posMin.x, posMin.y, posMax.z);

		m_pMesh->AddMeshQuad(lbPos, ltPos, rtPos, rbPos, indicebuffers, 4);
	}
	break;
	case Side_Left:
	{
		indicebuffers[0] = 1; indicebuffers[1] = 2; indicebuffers[2] = 5; indicebuffers[3] = 6;
		lbPos = posMin;
		ltPos = Vector3(posMin.x, posMax.y, posMin.z);
		rtPos = posMax;
		rbPos = Vector3(posMin.x, posMin.y, posMax.z);

		m_pMesh->AddMeshQuad(ltPos, rtPos, rbPos, lbPos, indicebuffers, 4);
	}
	break;
	case Side_Front:
	{
		indicebuffers[0] = 0; indicebuffers[1] = 1; indicebuffers[2] = 4; indicebuffers[3] = 5;
		lbPos = posMin;
		ltPos = Vector3(posMin.x, posMax.y, posMin.z);
		rtPos = posMax;
		rbPos = Vector3(posMax.x, posMin.y, posMin.z);

		m_pMesh->AddMeshQuad(ltPos, rtPos, rbPos, lbPos, indicebuffers, 4);
	}
	break;
	case Side_Back:
	{
		indicebuffers[0] = 2; indicebuffers[1] = 3; indicebuffers[2] = 6; indicebuffers[3] = 7; 
		lbPos = posMin;
		ltPos = Vector3(posMin.x, posMax.y, posMin.z);
		rtPos = posMax;
		rbPos = Vector3(posMax.x, posMin.y, posMin.z);

		m_pMesh->AddMeshQuad(lbPos, ltPos, rtPos, rbPos, indicebuffers, 4);
	}
	break;
	case Side_Top:
	{
		indicebuffers[0] = 0; indicebuffers[1] = 1; indicebuffers[2] = 2; indicebuffers[3] = 3;
		lbPos = posMin;
		ltPos = Vector3(posMin.x, posMin.y, posMax.z);
		rtPos = posMax;
		rbPos = Vector3(posMax.x, posMin.y, posMin.z);

		m_pMesh->AddMeshQuad(lbPos, ltPos, rtPos, rbPos, indicebuffers, 4);
	}

	break;
	case Side_Bottom:
	{
		indicebuffers[0] = 4; indicebuffers[1] = 5; indicebuffers[2] = 6; indicebuffers[3] = 7; 
		lbPos = posMin;
		ltPos = Vector3(posMin.x, posMin.y, posMax.z);
		rtPos = posMax;
		rbPos = Vector3(posMax.x, posMin.y, posMin.z);

		m_pMesh->AddMeshQuad(ltPos, rtPos, rbPos, lbPos, indicebuffers, 4);
	}

	break;
	}

}

}
