/********************************************************************
filename: 	AnimManager.h
file path:	dev\engine\Src\Core\Anim\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ANIMMANAGER_H__
#define __LORD_ANIMMANAGER_H__

#include "Resource/ResourceManager.h"
#include "Object/Singleton.h"

namespace LORD
{
	class Animation;
	class LORD_CORE_API AnimManager : public ResourceManager, public Singleton<AnimManager>
	{
	public:
		inline static AnimManager* Instance()
		{
			return ms_pSingleton;
		}

	public:
		AnimManager();
		virtual ~AnimManager();

	public:
		Animation*		createAnim(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);

	protected:
		Resource*	createImpl(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);
	};
}

#endif
