/********************************************************************
filename: 	ZipArchive.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ZIP_ARCHIVE_H__
#define __LORD_ZIP_ARCHIVE_H__

#include "Core.h"
#include "DataStream.h"
#include "Archive.h"
#include "ArchiveFactory.h"
#include "ResourcePack.h"

namespace LORD
{
	class LORD_CORE_API ZipArchive : public Archive 
	{
	public:
		ZipArchive(const String& strName, const String& strArchType);
		virtual ~ZipArchive();

		bool isCaseSensitive(void) const;

		void load();

		void unload();

		DataStream* open(const String& strFilename);

		StringArray list(bool bRecursive = true, bool bDirs = false);

		FileInfoList* listFileInfo(bool bDirs = false);

		bool exists(const String& strFilename);

	protected:
		ResourcePack		mResourcePack;
		bool				mLoaded;	

		//EE_MUTEX(mAutoMutex)
	};


	class LORD_CORE_API ZipArchiveFactory : public ArchiveFactory
	{
	public:
		virtual ~ZipArchiveFactory() {}

		const String& getType(void) const
		{
			static String strName = "Pak";
			return strName;
		}

		Archive *createInstance(const String& strName) 
		{
			return LordNew ZipArchive(strName, "Pak");
		}

		void destroyInstance(Archive* arch) 
		{ 
			LordDelete arch; 
		}
	};


	class LORD_CORE_API ResourcePackDataStream : public MemoryDataStream
	{
	public:
		ResourcePackDataStream(const String& strName, void* pvMem, size_t uSize, bool bFreeOnClose = false, bool bReadOnly = false)
			:	MemoryDataStream(strName, pvMem, uSize, bFreeOnClose, bReadOnly) {}
	};
}

#endif