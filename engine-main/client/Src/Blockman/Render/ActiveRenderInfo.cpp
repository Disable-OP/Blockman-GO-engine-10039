#include "ActiveRenderInfo.h"

#include "Entity/EntityPlayer.h"
#include "World/World.h"
#include "Block/Blocks.h"
#include "Block/BlockManager.h"
#include "Block/BM_Material.h"

namespace BLOCKMAN
{

Vector3 ActiveRenderInfo::objectPos(0.f, 0.f, 0.f);
float ActiveRenderInfo::rotationX = 0.f;
float ActiveRenderInfo::rotationXZ = 0.f;
float ActiveRenderInfo::rotationZ = 0.f;
float ActiveRenderInfo::rotationYZ = 0.f;
float ActiveRenderInfo::rotationXY = 0.f;

void ActiveRenderInfo::updateRenderInfo(EntityPlayer* pPlayer, bool thirdPersonView)
{
	int var4 = thirdPersonView ? 1 : 0;
	float pitch = pPlayer->rotationPitch * Math::DEG2RAD + Math::PI_DIV2;
	float yaw = pPlayer->rotationYaw * Math::DEG2RAD;
	rotationX = Math::Cos(yaw) * (1.f - var4 * 2.f);
	rotationZ = Math::Sin(yaw) * (1.f - var4 * 2.f);
	rotationYZ = rotationZ * Math::Cos(pitch) * (1.f - var4 * 2.f);
	rotationXY = -rotationX * Math::Cos(pitch) * (1.f - var4 * 2.f);
	rotationXZ = Math::Sin(pitch);
}

Vector3 ActiveRenderInfo::projectViewFromEntity(EntityLivingBase* pLiving, float rdt)
{
	Vector3 vec = pLiving->prevPos + (pLiving->position - pLiving->prevPos) * rdt;
	vec.y += pLiving->getEyeHeight();
	return vec + objectPos;
}

int ActiveRenderInfo::getBlockIdAtEntityViewpoint(World* pWorld, EntityLivingBase* pLiving, float rdt)
{
	Vector3 projVec = projectViewFromEntity(pLiving, rdt);
	BlockPos ipos = projVec.getFloor();
	int blockID = pWorld->getBlockId(ipos);

	if (blockID != 0 && BlockManager::sBlocks[blockID]->getMaterial().isLiquid())
	{
		float var6 = BlockFluid::getFluidHeightPercent(pWorld->getBlockMeta(ipos)) - 0.11111111F;
		float var7 = (float)(ipos.y + 1) - var6;

		if (projVec.y >= var7)
		{
			blockID = pWorld->getBlockId(ipos.getPosY());
		}
	}

	return blockID;
}

}