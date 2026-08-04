/********************************************************************
filename: 	EntitySpellFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-09-21
*********************************************************************/
#ifndef __ENTITY_SPELL_FX_HEADER__
#define __ENTITY_SPELL_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class EntitySpellFX : public EntityFx
{
protected:
	/** Base spell texture index */
	int baseSpellTextureIndex = 128;

public:
	EntitySpellFX(World* pWorld, const Vector3& pos1, const Vector3& pos2);

	/** implement override functions from EntityFx. */
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy) override;
	virtual void onUpdate() override;

	/** Sets the base spell texture index */
	void setBaseSpellTextureIndex(int idx) { baseSpellTextureIndex = idx; }
};

}

#endif
