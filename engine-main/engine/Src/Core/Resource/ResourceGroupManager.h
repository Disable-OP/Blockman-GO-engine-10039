/********************************************************************
filename: 	ResourceGroupManager.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RESOURCEGROUPMANAGER_H__
#define __LORD_RESOURCEGROUPMANAGER_H__

#include "Core.h"
#include "Object/Singleton.h"
#include "Archive.h"

namespace LORD
{
	class LORD_CORE_API ResourceGroupManager : public ObjectAlloc, public Singleton<ResourceGroupManager>
	{
	public:
		static ResourceGroupManager* Instance();

	public:
		EE_AUTO_MUTEX

		static String DEFAULT_RESOURCE_GROUP_NAME;
		static String INTERNAL_RESOURCE_GROUP_NAME;

		typedef list<Archive*>::type LocationList;

		typedef map<String, Archive*>::type ResourceLocationIndex;

		/// Resource group entry
		struct LORD_CORE_API ResourceGroup
		{
			EE_AUTO_MUTEX
				/// Group name
				String name;
			/// List of possible locations to search
			LocationList locationList;
			ResourceLocationIndex resourceIndexCaseSensitive;
			ResourceLocationIndex resourceIndexCaseInsensitive;

			void addToIndex(const String& filename, Archive* arch);
			void removeFromIndex(const String& filename, Archive* arch);
			void removeFromIndex(Archive* arch);
			const Archive* FindFileArchive(const String& filename);
		};

		/** Get resource group */
		ResourceGroup* getResourceGroup(const String& name);

	protected:
		/// Map from resource group names to groups
		typedef map<String, ResourceGroup*>::type ResourceGroupMap;
		ResourceGroupMap mResourceGroupMap;

		/** Delete a group for shutdown - don't notify ResourceManagers. */
		void deleteGroup(ResourceGroup* grp);
		/// Internal find method for auto groups
		ResourceGroup* findGroupContainingResourceImpl(const String& filename);

		/// Stored current group - optimisation for when bulk loading a group
		ResourceGroup* mCurrentGroup;
	public:
		ResourceGroupManager();
		virtual ~ResourceGroupManager();


		void createResourceGroup(const String& name);

		void addResourceLocation(const String& name, const String& locType, 
			const String& resGroup = DEFAULT_RESOURCE_GROUP_NAME);

		void removeResourceLocation(const String& name, 
			const String& resGroup = DEFAULT_RESOURCE_GROUP_NAME);

		DataStream* openResource(const String& resourceName, 
			const String& groupName = DEFAULT_RESOURCE_GROUP_NAME,
			bool searchGroupsIfNotFound = true);

		FileInfoList* listResourceFileInfo(const String& groupName);

		void releaseResourceFileInfo(FileInfoList* fileInfoList);

		bool resourceExists(const String& group, const String& filename);

		bool resourceExists(ResourceGroup* group, const String& filename);

		bool resourceExistsInAnyGroup(const String& filename);

		const String& findGroupContainingResource(const String& filename);

		String getFileLocation(const String& filename, 
			const String& groupName = DEFAULT_RESOURCE_GROUP_NAME,
			bool searchGroupsIfNotFound = true);

		StringArray getResourceGroups(void);

		const LocationList& getResourceLocationList(const String& groupName);

		const Archive* FindFileArchive(const String& filename);
	};
}

#endif
