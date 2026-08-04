#include "Core.h"
#include "ResourceGroupManager.h"
#include "Object/Exception.h"
#include "Archive.h"
#include "ArchiveManager.h"

namespace LORD
{

	String ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME = "General";
	String ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME = "Internal";

	//---------------------------------------------------------------------
	ResourceGroupManager* ResourceGroupManager::Instance()
	{
		return ms_pSingleton;
	}
	//-----------------------------------------------------------------------
	ResourceGroupManager::ResourceGroupManager()
		: mCurrentGroup(0)
	{
		// Create the 'General' group
		createResourceGroup(DEFAULT_RESOURCE_GROUP_NAME);
		// Create the 'Internal' group
		createResourceGroup(INTERNAL_RESOURCE_GROUP_NAME);
	}
	//-----------------------------------------------------------------------
	ResourceGroupManager::~ResourceGroupManager()
	{
		// delete all resource groups
		ResourceGroupMap::iterator i, iend;
		iend = mResourceGroupMap.end();
		for (i = mResourceGroupMap.begin(); i != iend; ++i)
		{
			deleteGroup(i->second);
		}
		mResourceGroupMap.clear();
	}
	//-----------------------------------------------------------------------
	void ResourceGroupManager::createResourceGroup(const String& name)
	{
		EE_LOCK_AUTO_MUTEX

			LordLogInfo("Creating resource group %s.", name.c_str());
		if(getResourceGroup(name))
		{
			LordException("Resource group with name [%s] already exists! ResourceGroupManager::createResourceGroup", name.c_str());
		}
		ResourceGroup* grp = LordNewT(ResourceGroup);
		grp->name = name;
		mResourceGroupMap.insert(
			ResourceGroupMap::value_type(name, grp));
	}
	//-----------------------------------------------------------------------
	void ResourceGroupManager::addResourceLocation(const String& name, 
		const String& locType, const String& resGroup)
	{
		ResourceGroup* grp = getResourceGroup(resGroup);
		if (!grp)
		{
			createResourceGroup(resGroup);
			grp = getResourceGroup(resGroup);
		}

		EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)

		// Get archive
		Archive* pArch = ArchiveManager::Instance()->load( name, locType );

		// Add to location list
		grp->locationList.push_back(pArch);
		// Index resources
		StringArray vec = pArch->list();
		for( StringArray::iterator it = vec.begin(); it != vec.end(); ++it )
			grp->addToIndex(*it, pArch);

		LordLogInfo("Added resource [%s] location of type %s to resource group [%s].", name.c_str(), locType.c_str(), resGroup.c_str());
	}
	//-----------------------------------------------------------------------
	void ResourceGroupManager::removeResourceLocation(const String& name, 
		const String& resGroup)
	{
		ResourceGroup* grp = getResourceGroup(resGroup);
		if (!grp)
		{
			LordLogError("Cannot locate a resource group called [%s] ResourceGroupManager::removeResourceLocation.", resGroup.c_str());
			return ;
		}

		EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)

			// Remove from location list
			LocationList::iterator li, liend;
		liend = grp->locationList.end();
		for (li = grp->locationList.begin(); li != liend; ++li)
		{
			Archive* pArch = *li;
			if (pArch->getName() == name)
			{
				grp->removeFromIndex(pArch);
				grp->locationList.erase(li);

				break;
			}
		}

		LordLogInfo("Removed resource [%s] location", name.c_str());
	}
	//-----------------------------------------------------------------------
	DataStream* ResourceGroupManager::openResource(
		const String& resourceName, const String& groupName, 
		bool searchGroupsIfNotFound)
	{
		EE_LOCK_AUTO_MUTEX

			// Try to find in resource index first
			ResourceGroup* grp = getResourceGroup(groupName);
		if (!grp)
		{
			LordLogError("Cannot locate a resource group called [%s] for resource [%s] ResourceGroupManager::openResource", groupName.c_str(), resourceName.c_str());
			return NULL;
		}

		EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)

		Archive *pArch = NULL;
		ResourceLocationIndex::iterator rit = grp->resourceIndexCaseSensitive.find(resourceName);
		if (rit != grp->resourceIndexCaseSensitive.end())
		{
			// Found in the index
			pArch = rit->second;
			DataStream* stream = pArch->open(resourceName);
			return stream;
		}
		else 
		{
			// try case insensitive
			String lcResourceName = resourceName;
			StringUtil::LowerCase(lcResourceName);
			rit = grp->resourceIndexCaseInsensitive.find(lcResourceName);
			if (rit != grp->resourceIndexCaseInsensitive.end())
			{
				// Found in the index
				pArch = rit->second;
				DataStream* stream = pArch->open(lcResourceName);
				return stream;
			}
		}

		if (searchGroupsIfNotFound)
		{
			ResourceGroup* grp = findGroupContainingResourceImpl(resourceName); 
			if (grp)
			{
				return openResource(resourceName, grp->name, false);
			}
			else
			{
				LordLogError("Cannot locate a resource [%s] in resource group [%s] or any other group. ResourceGroupManager::openResource", resourceName.c_str(), groupName.c_str());
				return NULL;
			}
		}

		LordLogError("Cannot locate a resource %s in resource group %s ResourceGroupManager::openResource", resourceName.c_str(), groupName.c_str());
		return NULL; 
	}
	//-----------------------------------------------------------------------
	ResourceGroupManager::ResourceGroup* ResourceGroupManager::getResourceGroup(const String& name)
	{
		EE_LOCK_AUTO_MUTEX

			ResourceGroupMap::iterator i = mResourceGroupMap.find(name);
		if (i != mResourceGroupMap.end())
		{
			return i->second;
		}
		return 0;

	}
	//-----------------------------------------------------------------------
	void ResourceGroupManager::deleteGroup(ResourceGroup* grp)
	{
		// delete ResourceGroup
		LordDeleteT(grp, ResourceGroup);
	}
	//-----------------------------------------------------------------------
	void ResourceGroupManager::releaseResourceFileInfo(FileInfoList* fileInfoList)
	{
		LordFree(fileInfoList);
	}
	//-----------------------------------------------------------------------
	FileInfoList* ResourceGroupManager::listResourceFileInfo(const String& groupName)
	{
		EE_LOCK_AUTO_MUTEX
			// MEMCATEGORY_GENERAL is the only category supported for SharedPtr
			FileInfoList* vec = LordNewT(FileInfoList);

		// Try to find in resource index first
		ResourceGroup* grp = getResourceGroup(groupName);
		if (!grp)
		{
			LordException("Error: Cannot locate a resource group called %s ResourceGroupManager::listResourceFileInfo", groupName.c_str());
		}

		EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)

			// Iterate over the archives
			LocationList::iterator i, iend;
		iend = grp->locationList.end();
		for(i = grp->locationList.begin(); i != iend; ++i)
		{
			FileInfoList* lst = (*i)->listFileInfo();
			vec->insert(vec->end(), lst->begin(), lst->end());
			LordFree(lst);
			lst = NULL;
		}

		return vec;
	}
	//-----------------------------------------------------------------------
	bool ResourceGroupManager::resourceExists(const String& groupName, const String& resourceName)
	{
		EE_LOCK_AUTO_MUTEX

			// Try to find in resource index first
			ResourceGroup* grp = getResourceGroup(groupName);
		if (!grp)
		{
			LordException("Error: Cannot locate a resource group called %s. ResourceGroupManager::openResource", groupName.c_str());
		}

		return resourceExists(grp, resourceName);
	}
	//-----------------------------------------------------------------------
	bool ResourceGroupManager::resourceExists(ResourceGroup* grp, const String& resourceName)
	{
		EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)

			// Try indexes first
			ResourceLocationIndex::iterator rit = grp->resourceIndexCaseSensitive.find(resourceName);
		if (rit != grp->resourceIndexCaseSensitive.end())
		{
			// Found in the index
			return true;
		}
		else 
		{
			// try case insensitive
			String lcResourceName = resourceName;
			StringUtil::LowerCase(lcResourceName);
			rit = grp->resourceIndexCaseInsensitive.find(lcResourceName);
			if (rit != grp->resourceIndexCaseInsensitive.end())
			{
				// Found in the index
				return true;
			}
		}
		return false;
	}
	//-----------------------------------------------------------------------
	ResourceGroupManager::ResourceGroup* 
		ResourceGroupManager::findGroupContainingResourceImpl(const String& filename)
	{
		EE_LOCK_AUTO_MUTEX

			// Iterate over resource groups and find
			for (ResourceGroupMap::iterator i = mResourceGroupMap.begin();
				i != mResourceGroupMap.end(); ++i)
			{
				ResourceGroup* grp = i->second;

				EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)

					if (resourceExists(grp, filename))
						return grp;
			}
			// Not found
			return 0;
	}
	//-----------------------------------------------------------------------
	bool ResourceGroupManager::resourceExistsInAnyGroup(const String& filename)
	{
		ResourceGroup* grp = findGroupContainingResourceImpl(filename);
		if (!grp)
			return false;
		return true;
	}
	//-----------------------------------------------------------------------
	const String& ResourceGroupManager::findGroupContainingResource(const String& filename)
	{
		ResourceGroup* grp = findGroupContainingResourceImpl(filename);
		if (!grp)
		{
			LordException("Error: Unable to derive resource group for %s automatically since the resource was not found. ResourceGroupManager::findGroupContainingResource", filename.c_str());
		}
		return grp->name;
	}
	//-----------------------------------------------------------------------
	String ResourceGroupManager::getFileLocation(const String& filename, const String& groupName, bool searchGroupsIfNotFound)
	{
		String ret;
		ResourceGroup* grp = getResourceGroup(groupName);
		if (!grp)
		{
			LordLogError("Cannot locate a resource group called [%s] for resource [%s] ResourceGroupManager::openResource", groupName.c_str(), filename.c_str());
			return ret;
		}

		EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)

		Archive *pArch = NULL;
		ResourceLocationIndex::iterator rit = grp->resourceIndexCaseSensitive.find(filename);
		if (rit != grp->resourceIndexCaseSensitive.end())
		{
			// Found in the index
			pArch = rit->second;
			ret = pArch->location(filename);
			return ret;
		}
		else 
		{
			// try case insensitive
			String lcResourceName = filename;
			StringUtil::LowerCase(lcResourceName);
			rit = grp->resourceIndexCaseInsensitive.find(lcResourceName);
			if (rit != grp->resourceIndexCaseInsensitive.end())
			{
				// Found in the index
				pArch = rit->second;
				ret = pArch->location(filename);
				return ret;
			}
		}

		if (searchGroupsIfNotFound)
		{
			ResourceGroup* grp = findGroupContainingResourceImpl(filename); 
			if (grp)
			{
				return getFileLocation(filename, grp->name, false);
			}
			else
			{
				LordLogError("Cannot locate a resource [%s] in resource group [%s] or any other group. ResourceGroupManager::openResource", filename.c_str(), groupName.c_str());
				return ret;
			}
		}

		LordLogError("Cannot locate a resource %s in resource group %s ResourceGroupManager::openResource", filename.c_str(), groupName.c_str());
		return ret; 
	}
	//-----------------------------------------------------------------------
	StringArray ResourceGroupManager::getResourceGroups(void)
	{
		EE_LOCK_AUTO_MUTEX

			StringArray vec;
		for (ResourceGroupMap::iterator i = mResourceGroupMap.begin();
			i != mResourceGroupMap.end(); ++i)
		{
			vec.push_back(i->second->name);
		}
		return vec;
	}
	//---------------------------------------------------------------------
	const ResourceGroupManager::LocationList& 
		ResourceGroupManager::getResourceLocationList(const String& group)
	{
		EE_LOCK_AUTO_MUTEX

			ResourceGroup* grp = getResourceGroup(group);
		if (!grp)
		{
			LordException("Error: Cannot locate a resource group called %s ResourceGroupManager::getResourceLocationList", group.c_str());
		}

		EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)
			return grp->locationList;
	}

	const Archive* ResourceGroupManager::FindFileArchive(const String& filename)
	{
		EE_LOCK_AUTO_MUTEX

			// Iterate over resource groups and find
		for (ResourceGroupMap::iterator i = mResourceGroupMap.begin();
			i != mResourceGroupMap.end(); ++i)
		{
			ResourceGroup* grp = i->second;

			EE_LOCK_MUTEX(grp->AUTO_MUTEX_NAME)
				const Archive* pArchive = grp->FindFileArchive(filename);
			if (pArchive)
				return pArchive;
		}
		// Not found
		return 0;
	}

	//---------------------------------------------------------------------
	void ResourceGroupManager::ResourceGroup::addToIndex(const String& filename, Archive* arch)
	{
		// internal, assumes mutex lock has already been obtained
		this->resourceIndexCaseSensitive[filename] = arch;

		if (!arch->isCaseSensitive())
		{
			String lcase = filename;
			StringUtil::LowerCase(lcase);
			this->resourceIndexCaseInsensitive[lcase] = arch;
		}
	}
	//---------------------------------------------------------------------
	void ResourceGroupManager::ResourceGroup::removeFromIndex(const String& filename, Archive* arch)
	{
		// internal, assumes mutex lock has already been obtained
		ResourceLocationIndex::iterator i = this->resourceIndexCaseSensitive.find(filename);
		if (i != this->resourceIndexCaseSensitive.end() && i->second == arch)
			this->resourceIndexCaseSensitive.erase(i);

		if (!arch->isCaseSensitive())
		{
			String lcase = filename;
			StringUtil::LowerCase(lcase);
			i = this->resourceIndexCaseInsensitive.find(filename);
			if (i != this->resourceIndexCaseInsensitive.end() && i->second == arch)
				this->resourceIndexCaseInsensitive.erase(i);
		}
	}
	//---------------------------------------------------------------------
	void ResourceGroupManager::ResourceGroup::removeFromIndex(Archive* arch)
	{
		// Delete indexes
		ResourceLocationIndex::iterator rit, ritend;
		ritend = this->resourceIndexCaseInsensitive.end();
		for (rit = this->resourceIndexCaseInsensitive.begin(); rit != ritend;)
		{
			if (rit->second == arch)
			{
				ResourceLocationIndex::iterator del = rit++;
				this->resourceIndexCaseInsensitive.erase(del);
			}
			else
			{
				++rit;
			}
		}
		ritend = this->resourceIndexCaseSensitive.end();
		for (rit = this->resourceIndexCaseSensitive.begin(); rit != ritend;)
		{
			if (rit->second == arch)
			{
				ResourceLocationIndex::iterator del = rit++;
				this->resourceIndexCaseSensitive.erase(del);
			}
			else
			{
				++rit;
			}
		}
	}

	const Archive* ResourceGroupManager::ResourceGroup::FindFileArchive(const String& filename)
	{
		ResourceLocationIndex::iterator rit = resourceIndexCaseInsensitive.find(filename);
		if(rit != resourceIndexCaseInsensitive.end())
		{
			return rit->second;
		}

		rit = resourceIndexCaseSensitive.find(filename);
		if(rit != resourceIndexCaseSensitive.end())
		{
			return rit->second;
		}

		return NULL;
	}
}

