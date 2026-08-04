/*
from Ogre
*/
#include "Core.h"
#include "SkeletonManager.h"
#include "Skeleton.h"

namespace LORD
{
	//---------------------------------------------------------------------
	/*SkeletonManager* SkeletonManager::Instance()
	{
		return ms_pSingleton;
	}*/

	SkeletonManager::SkeletonManager()
	{
	}

	SkeletonManager::~SkeletonManager()
	{
		// subclasses should unregister with resource group manager
	}

	Skeleton* SkeletonManager::createSkeleton(const String& name, ResourceType rt)
	{
		Skeleton* pSkeleton =(Skeleton*)ResourceManager::createResource(name, rt);
		return pSkeleton;
	}

	Resource* SkeletonManager::createImpl(const String& name, ResourceType rt)
	{
		return LordNew Skeleton(name, rt);
	}
}
