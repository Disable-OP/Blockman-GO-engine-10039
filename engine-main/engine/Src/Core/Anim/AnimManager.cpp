#include "Core.h"
#include "AnimManager.h"
#include "Animation.h"

namespace LORD
{
	//---------------------------------------------------------------------
	/*AnimManager* AnimManager::Instance()
	{
		return ms_pSingleton;
	}*/

	AnimManager::AnimManager()
	{
	}

	AnimManager::~AnimManager()
	{
		// subclasses should unregister with resource group manager
	}

	Animation* AnimManager::createAnim(const String& name, ResourceType rt)
	{
		Animation* pAnimation =(Animation*)ResourceManager::createResource(name, rt);
		return pAnimation;
	}

	Resource* AnimManager::createImpl(const String& name, ResourceType rt)
	{
		return LordNew Animation(name, rt);
	}

}