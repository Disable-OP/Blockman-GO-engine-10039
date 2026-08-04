/********************************************************************
filename: 	SkeletonManager.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_SKELETONMANAGER_H__
#define __LORD_SKELETONMANAGER_H__

#include "Resource/ResourceManager.h"
#include "Object/Singleton.h"

namespace LORD
{
	class Skeleton;
	class LORD_CORE_API SkeletonManager : public ResourceManager, public Singleton<SkeletonManager>
	{
	public:
		inline static SkeletonManager* Instance()
		{
			return ms_pSingleton;
		}

	public:
		SkeletonManager();
		virtual ~SkeletonManager();

		Skeleton* createSkeleton(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);

	protected:
		/// @copydoc ResourceManager::createImpl
		Resource* createImpl(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);
	};

}

#endif
