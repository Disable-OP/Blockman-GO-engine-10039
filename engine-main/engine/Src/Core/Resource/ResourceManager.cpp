#include "Core.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "Object/Exception.h"
#include "Util/AssertX.h"
#include "Archive.h"
#include "ArchiveManager.h"
#include "ResourceGroupManager.h"

namespace LORD
{
	ResourceManager::ResourceManager()
		: mMemoryUsage(0)
		, mReleaseDelayTime(5000)
	{
	}
	//-----------------------------------------------------------------------
	ResourceManager::~ResourceManager()
	{
		ResourceMap::iterator it = mResources.begin();
		for(; it != mResources.end(); ++it)
		{
			LordSafeDelete (it->second);
		}
		mResources.clear();

		it = mResources_delay.begin();
		for(; it!=mResources_delay.end(); ++it)
		{
			LordSafeDelete (it->second);
		}
	}

	Resource* ResourceManager::createResource(const String &name, ResourceType rt)
	{
		ResourceCreateOrRetrieveResult res = ResourceManager::createOrRetrieve(name, rt);
		return res.first;
	}

	//-----------------------------------------------------------------------
	Resource* ResourceManager::createNewResource(const String& name, ResourceType rt)
	{
		// Call creation implementation
		Resource* ret = createImpl(name, rt);
		ret->addRefrenceCount();

		mResources.insert( ResourceMap::value_type(name, ret));
		return ret;
	}
	//-----------------------------------------------------------------------
	ResourceManager::ResourceCreateOrRetrieveResult 
		ResourceManager::createOrRetrieve(const String& name, ResourceType rt)
	{
		Resource* res = getByName(name);
		bool created = false;
		if (res == NULL)
		{
			ResourceMap::iterator it = mResources_delay.find(name);
			if( it != mResources_delay.end())
			{
				res = it->second;
			}

			if(res == NULL)
			{
				created = true;
				res = createNewResource(name, rt);
			}
			else
			{
				mResources_delay.erase(it);
				mResources.insert( ResourceMap::value_type(name, res));
				res->reserveDelayed();
				res->addRefrenceCount();
			}
		}
		else
		{
			res->addRefrenceCount();
		}
		return ResourceCreateOrRetrieveResult(res, created);
	}
	//-----------------------------------------------------------------------
	bool ResourceManager::releaseResource(Resource* r, bool isDelayed)
	{
		if(r == NULL)
		{
			return false;
		}

		if(r->subRefrenceCount())
		{
			ResourceMap::iterator nameIt = mResources.find(r->getName());
			if (nameIt != mResources.end())
			{
				mResources.erase(nameIt);
			}
			else
			{
				LordAssertX(0, "Error: delete wide point.");
			}
			
			if (isDelayed)
			{
				r->delayedRelease();
				mResources_delay.insert(ResourceMap::value_type(r->getName(), r));
			} 
			else
			{
				r->unload();
				LordDelete r;
			}

			return true;
		}

		return false;
	}
	//-----------------------------------------------------------------------
	Resource* ResourceManager::getByName(const String& name)
	{		
		Resource* res = NULL;

		ResourceMap::iterator it = mResources.find(name);
		if( it != mResources.end())
		{
			res = it->second;
		}

		return res;
	}
	//-----------------------------------------------------------------------
	void ResourceManager::setReleaseDelayTime(ui32 t)
	{
		mReleaseDelayTime = t;
	}
	//-----------------------------------------------------------------------
	ui32 ResourceManager::getReleaseDelayTime() const
	{
		return mReleaseDelayTime;
	}
	//-----------------------------------------------------------------------
	void ResourceManager::updateDelayedRelease(ui32 frameTime)
	{
		ResourceMap::iterator it = mResources_delay.begin(), tempIt;
		
		for(; it!=mResources_delay.end(); )
		{
			Resource* res = it->second;
			if(res->addDelayedTime(frameTime) >= mReleaseDelayTime)
			{
				res->unload();
				LordDelete res;
				tempIt = it++;
				mResources_delay.erase(tempIt);
			}
			else
			{
				++it;
			}
		}
	}
	//-----------------------------------------------------------------------
	void ResourceManager::releaseAllDelayed()
	{
		ResourceMap::iterator it = mResources_delay.begin();
		
		for(; it!=mResources_delay.end(); ++it)
		{
			Resource* res = it->second;
			res->unload();
			LordDelete res;
		}
		mResources_delay.clear();
	}
	//-----------------------------------------------------------------------
	void ResourceManager::outputResourceLog(const String& managerType)
	{
		ResourceMap::iterator it = mResources.begin();
		LordLogInfo("%s Resource Leak Begin.......", managerType.c_str());
		for(; it!=mResources.end(); ++it)
		{
			if(it->second->getLoadingState() != Resource::LOADSTATE_CREATED)
				LordLogInfo("%s", it->first.c_str());
		}
		LordLogInfo("%s Resource Leak End.......", managerType.c_str());
	}

	bool ResourceManager::validResource(Resource* r)
	{
		ResourceMap::iterator it = mResources.begin();
		for (; it != mResources.end(); ++it)
		{
			if (it->second == r)
			{
				return true;
			}
		}
		return false;
	}

}


