/********************************************************************
filename: 	EntityBreakingFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-10-11
*********************************************************************/
#ifndef __ENTITY_BREAKING_FX_HEADER__
#define __ENTITY_BREAKING_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class cItem;

class EntityBreakingFX : public EntityFx
{
public:
	EntityBreakingFX(World* pWorld, const Vector3& pos, cItem* pItem, int subtype = 0);
	EntityBreakingFX(World* pWorld, const Vector3& pos, const Vector3& pos2, cItem* pItem, int subtype = 0);

	/** implement override functions from EntityFx. */
	virtual int getFXLayer() override { return 2; }
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy) override;
};

}

#endif