/*
from Ogre
*/
#include "Core.h"

#include "ArchiveManager.h"

#include "ArchiveFactory.h"
#include "Archive.h"
#include "Object/Exception.h"

namespace LORD
{
	typedef void (*createFunc)( Archive**, const String& );

	//---------------------------------------------------------------------

	ArchiveManager* ArchiveManager::Instance()
	{
		return ms_pSingleton;
	}

	//-----------------------------------------------------------------------
	Archive* ArchiveManager::load( const String& filename, const String& archiveType)
	{
		ArchiveMap::iterator i = mArchives.find(filename);
		Archive* pArch = 0;

		if (i == mArchives.end())
		{
			// Search factories
			ArchiveFactoryMap::iterator it = mArchFactories.find(archiveType);
			if (it == mArchFactories.end())
			{
				LordException("Error: Cannot find an archive factory  to deal with archive of type %s in ArchiveManager::load", archiveType.c_str());
			}
			pArch = it->second->createInstance(filename);
			pArch->load();
			mArchives[filename] = pArch;

		}
		else
		{
			pArch = i->second;
		}
		return pArch;
	}
	//-----------------------------------------------------------------------
	void ArchiveManager::unload(Archive* arch)
	{
		unload(arch->getName());
	}
	//-----------------------------------------------------------------------
	void ArchiveManager::unload(const String& filename)
	{
		ArchiveMap::iterator i = mArchives.find(filename);

		if (i != mArchives.end())
		{
			i->second->unload();
			// Find factory to destroy
			ArchiveFactoryMap::iterator fit = mArchFactories.find(i->second->getType());
			if (fit == mArchFactories.end())
			{
				// Factory not found
				LordException("Error: Cannot find an archive factory to deal with  archive of type %s in ArchiveManager::~ArchiveManager", i->second->getType().c_str());
			}
			fit->second->destroyInstance(i->second);
			mArchives.erase(i);
		}
	}
	//-----------------------------------------------------------------------
	ArchiveManager::ArchiveMap& ArchiveManager::getArchiveIterator(void)
	{
		return mArchives;
	}
	//-----------------------------------------------------------------------
	ArchiveManager::~ArchiveManager()
	{
		// Unload & delete resources in turn
		for( ArchiveMap::iterator it = mArchives.begin(); it != mArchives.end(); ++it )
		{
			Archive* arch = it->second;
			// Unload
			arch->unload();
			// Find factory to destroy
			ArchiveFactoryMap::iterator fit = mArchFactories.find(arch->getType());
			if (fit == mArchFactories.end())
			{
				// Factory not found
				LordException("Error: Cannot find an archive factory to deal with archive of type %s in ArchiveManager::~ArchiveManager", arch->getType().c_str());
			}
			fit->second->destroyInstance(arch);

		}
		// Empty the list
		mArchives.clear();
	}
	//-----------------------------------------------------------------------
	void ArchiveManager::addArchiveFactory(ArchiveFactory* factory)
	{        
		mArchFactories.insert( ArchiveFactoryMap::value_type( factory->getType(), factory ) );
		LordLogInfo("ArchiveFactory for archive type %s registered.", factory->getType().c_str());
	}
}
