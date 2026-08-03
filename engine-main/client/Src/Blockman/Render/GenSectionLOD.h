/********************************************************************
filename: 	RenderBlocks.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-30
*********************************************************************/
#ifndef __RENDER_GEN_SECTION_LOD__
#define __RENDER_GEN_SECTION_LOD__

#include "BM_TypeDef.h"

namespace BLOCKMAN
{

class SectionRenderer;
class RasterizerMesh;
class ChunkClient;

enum Side
{
	Side_Right = 0,
	Side_Left = 1,
	Side_Front = 2,
	Side_Back = 3,
	Side_Top = 4,
	Side_Bottom = 5,
	Side_Count = 6,
};

struct Stripe
{
	Vector2i bias2D;

	ui8 targetID;
	ui8 length;

	Stripe() 
	{
		bias2D.x = 999;
		bias2D.y = 999; 
		length = 0;
		targetID = 255;
	}
	void Reset(const Vector2i &i_bias2D, int i_length, ui8 i_targetID)
	{
		bias2D = i_bias2D;
		length = i_length;
		targetID = i_targetID;
	}
	void Reset()
	{
		bias2D = Vector2i(999, 999);
		length = 0;
		targetID = 255;
		
	}
	bool CanMerge(Stripe &other)
	{
		return (other.targetID == targetID && length == other.length);
	}
};

class GenSectionLod : public ObjectAlloc
{
public:
	GenSectionLod(RasterizerMesh* pMesh, const Vector3i& sectionBegin);
	~GenSectionLod();

	void generate(ui32 rawdata_lod[][20][20]);

	void AddQuad(ui8 opaqueFlag, Vector3 min, Vector3 max, Side side, Stripe& stripe);

	inline bool ShouldGenCubeFace(ui8 curop, ui8 adjop)
	{
		return curop != 0 && adjop == 0;
	}

	int solidCnt = 0;

protected:
	RasterizerMesh* m_pMesh;
	Vector3i m_sectionId;
};

}

#endif
