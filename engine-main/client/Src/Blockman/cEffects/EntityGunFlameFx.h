/********************************************************************
filename: 	EntityGunFlameFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2018-01-12
*********************************************************************/
#ifndef __ENTITY_GUN_FLAME_FX_HEADER__
#define __ENTITY_GUN_FLAME_FX_HEADER__

#include "EntityFx.h"

namespace BLOCKMAN
{

class cBlock;
class EntityPlayer;

class EntityGunFlameFX : public EntityFx
{

public:
	EntityGunFlameFX(World* pWorld, const Vector3& pos1, EntityPlayer* shooter);
	~EntityGunFlameFX();

	/** implement override functions from EntityFx. */
	virtual void onUpdate() override;
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy);

protected:
	EntityPlayer* m_shooter;
	int m_frameIndex;

	LineRenderable* m_line = nullptr;
};

}

#endif
