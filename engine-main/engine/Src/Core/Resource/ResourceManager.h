/********************************************************************
filename: 	ResourceManager.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RESOURCE_MANAGER_H__
#define __LORD_RESOURCE_MANAGER_H__

#include "Core.h"
#include "Resource.h"

namespace LORD
{
	class LORD_CORE_API ResourceManager : public ObjectAlloc
	{
	public:
		ResourceManager();
		virtual ~ResourceManager();

		typedef map< String, Resource* >::type ResourceMap;

	public:

		typedef std::pair<Resource*, bool> ResourceCreateOrRetrieveResult;

		virtual size_t getMemoryUsage(void) const { return mMemoryUsage; }

		// create resource
		virtual Resource* createResource(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);

		virtual bool releaseResource(Resource* r, bool isDelayed = true);

		virtual Resource* getByName(const String& name);

		void setReleaseDelayTime(ui32 t);

		ui32 getReleaseDelayTime() const;

		void updateDelayedRelease(ui32 frameTime);

		void releaseAllDelayed();

		void outputResourceLog(const String& managerType);

		bool validResource(Resource* r);

	protected:

		virtual Resource* createNewResource(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);

		virtual ResourceCreateOrRetrieveResult createOrRetrieve(const String& name, ResourceType rt = RESOURCE_TYPE_NORMAL);

		virtual Resource* createImpl(const String& name, ResourceType rt) = 0;

	protected:
		ResourceMap mResources;
		ResourceMap mResources_delay;
		size_t		mMemoryUsage; // In bytes
		ui32		mReleaseDelayTime;
	};
}

#endif
