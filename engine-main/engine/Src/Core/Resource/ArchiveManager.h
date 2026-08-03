/********************************************************************
filename: 	ArchiveManager.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ARCHIVEMANAGER_H__
#define __LORD_ARCHIVEMANAGER_H__

#include "Core.h"
#include "Object/Singleton.h"

namespace LORD
{
	class LORD_CORE_API ArchiveManager : public ObjectAlloc, public Singleton<ArchiveManager>
	{
	public:
		static ArchiveManager* Instance();

	protected:
		typedef map<String, ArchiveFactory*>::type ArchiveFactoryMap;
		/// Factories available to create archives, indexed by archive type (String identifier e.g. 'Zip')
		ArchiveFactoryMap mArchFactories;
		/// Currently loaded archives
		typedef map<String, Archive*>::type ArchiveMap;
		ArchiveMap mArchives;

	public:
		/** Default constructor - should never get called by a client app.
		*/
		ArchiveManager() {}
		/** Default destructor.
		*/
		virtual ~ArchiveManager();

		/** Opens an archive for file reading.
		@remarks
		The archives are created using class factories within
		extension libraries.
		@param filename
		The filename that will be opened
		@param refLibrary
		The library that contains the data-handling code
		@returns
		If the function succeeds, a valid pointer to an Archive
		object is returned.
		@par
		If the function fails, an exception is thrown.
		*/
		Archive* load( const String& filename, const String& archiveType);

		/** Unloads an archive.
		@remarks
		You must ensure that this archive is not being used before removing it.
		*/
		void unload(Archive* arch);
		/** Unloads an archive by name.
		@remarks
		You must ensure that this archive is not being used before removing it.
		*/
		void unload(const String& filename);

		/** Get an iterator over the Archives in this Manager. */
		ArchiveMap& getArchiveIterator(void);

		void addArchiveFactory(ArchiveFactory* factory);

	};
}

#endif
