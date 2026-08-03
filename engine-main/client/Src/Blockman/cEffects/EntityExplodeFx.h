/********************************************************************
filename: 	EntityHugeExplodeFx.h
file path:	dev\client\Src\Blockman\Effects

version:	1
author:		ajohn
company:	supernano
date:		2017-10-30
*********************************************************************/
#ifndef __ENTITY_EXPLODE_FX_H__
#define __ENTITY_EXPLODE_FX_H__

#include "EntityFx.h"

namespace BLOCKMAN
{

class EntityHugeExplodeFX : public EntityFx
{
public:
	EntityHugeExplodeFX(World* pWorld, const Vector3& pos, const Vector3& vel);

	/** implement override functions from EntityFx. */
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy) override {}
	virtual void onUpdate() override;
	virtual int getFXLayer() override { return 4; }

protected:
	int m_timeSinceStart = 0;
	int m_maximumTime = 0;
};

class EntityLargeExplodeFX : public EntityFx
{
public:
	EntityLargeExplodeFX(World* pWorld, const Vector3& pos, const Vector3& vel);
	
	/** implement override functions from Entity. */
	virtual int getBrightnessForRender(float rdt) override { return 0xF0F0; }

	/** implement override functions from EntityFx. */
	virtual void renderParticle(FxVertexFmt* vertices, int brightness, float rdt, float rx, float rxz, float rz, float ryz, float rxy) override;
	virtual void onUpdate() override;
	virtual int getFXLayer() override {	return 3; }

protected:
	int m_explodeAge = 0;
	int m_maxExplodeAge = 0;
	float m_scales = 0.f;
};

}

#endif