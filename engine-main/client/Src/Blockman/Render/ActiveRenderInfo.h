/********************************************************************
filename: 	ActiveRenderInfo.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2017-08-02
*********************************************************************/
#ifndef __ACTIVE_RENDER_INFO_HEADER__
#define __ACTIVE_RENDER_INFO_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class World;
class EntityLivingBase;
class EntityPlayer;

class ActiveRenderInfo : public ObjectAlloc
{
public:
	/** The calculated view object X coordinate */
	static Vector3 objectPos;
	
	/** The X component of the entity's yaw rotation */
	static float rotationX;

	/** The combined X and Z components of the entity's pitch rotation */
	static float rotationXZ;

	/** The Z component of the entity's yaw rotation */
	static float rotationZ;

	/** The Y component (scaled along the Z axis) of the entity's pitch rotation */
	static float rotationYZ;

	/** The Y component (scaled along the X axis) of the entity's pitch rotation */
	static float rotationXY;

	/** Updates the current render info and camera location based on entity look angles and 1st/3rd person view mode */
	static void updateRenderInfo(EntityPlayer* pPlayer, bool thirdPersonView);
	
	/** Returns a vector representing the projection along the given entity's view for the given distance */
	static Vector3 projectViewFromEntity(EntityLivingBase* pLiving, float rdt);
	
	/** Returns the block ID at the current camera location (either air or fluid), taking into account the height of fluid blocks */
	static int getBlockIdAtEntityViewpoint(World* pWorld, EntityLivingBase* pLiving, float rdt);
};

}

#endif
