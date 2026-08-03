/********************************************************************
filename: 	APKFileSystem.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __APKFileSystemArchive_H__
#define __APKFileSystemArchive_H__

#if LORD_PLATFORM == LORD_PLATFORM_ANDROID

#include "Archive.h"
#include "ArchiveManager.h"
#include "ArchiveFactory.h"
#include <android/asset_manager.h>

namespace LORD
{
	class LORD_CORE_API APKFileSystemArchive : public Archive
	{
	private:
		AAssetManager* mAssetMgr;
		String mPathPreFix;

	public:
		APKFileSystemArchive(const String& name, const String& archType, AAssetManager* assetMgr);
		~APKFileSystemArchive();

		/// @copydoc Archive::isCaseSensitive
		bool isCaseSensitive(void) const;

		/// @copydoc Archive::load
		void load();
		/// @copydoc Archive::unload
		void unload();

		/// @copydoc Archive::open
		DataStream* open(const String& filename);

		/// @copydoc Archive::list
		StringArray list(bool recursive = true, bool dirs = false);

		/// @copydoc Archive::listFileInfo
		FileInfoList* listFileInfo(bool dirs = false);

		/// @copydoc Archive::exists
		bool exists(const String& filename);
	};

	class LORD_CORE_API APKFileSystemArchiveFactory : public ArchiveFactory
	{
	public:
		APKFileSystemArchiveFactory(AAssetManager* assetMgr) 
			: mAssetMgr(assetMgr) 
		{}

		virtual ~APKFileSystemArchiveFactory()
		{}

		/// @copydoc FactoryObj::getType
		const String& getType(void) const;
		/// @copydoc FactoryObj::createInstance
		Archive* createInstance(const String& name)
		{
			return LordNew APKFileSystemArchive(name, getType(), mAssetMgr);
		}
		/// @copydoc FactoryObj::destroyInstance
		void destroyInstance(Archive* arch)
		{
			LordDelete arch; 
		}

	private:
		AAssetManager* mAssetMgr;
	};
}

#endif
#endif
