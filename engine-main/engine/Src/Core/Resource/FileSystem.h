/********************************************************************
filename: 	FileSystem.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __FileSystem_H__
#define __FileSystem_H__

#include "Archive.h"
#include "ArchiveFactory.h"

namespace LORD
{
	
class LORD_CORE_API FileSystemArchive : public Archive 
{

public:
	FileSystemArchive(const String& name, const String& archType );
	~FileSystemArchive();

	bool isCaseSensitive(void) const;

	void load();

	void unload();

	DataStream* open(const String& filename);

	String location(const String& filename);

	StringArray list(bool recursive = true, bool dirs = false);

	FileInfoList* listFileInfo(bool dirs = false);

	StringArray find(const String& pattern, bool recursive = true,
		bool dirs = false);

	void findFiles(const String& pattern, bool dirs,
		StringArray* simpleList, FileInfoList* detailList);

	bool exists(const String& filename);
};

class LORD_CORE_API FileSystemArchiveFactory : public ArchiveFactory
{
public:
	virtual ~FileSystemArchiveFactory() {}

	const String& getType(void) const;

	Archive* createInstance(const String& name) 
	{
		return LordNew FileSystemArchive(name, "FileSystem");
	}

	void destroyInstance(Archive* arch) { LordDelete arch; }
};
}

#endif
