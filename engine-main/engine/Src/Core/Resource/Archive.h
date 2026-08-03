/********************************************************************
filename: 	Archive.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef _LORD_ARCHIVE_H__
#define _LORD_ARCHIVE_H__

#include "DataStream.h"

namespace LORD
{
	class Archive;
	/** Information about a file/directory within the archive will be
	returned using a FileInfo struct.
	@see
	Archive
	*/
	struct FileInfo {
		/// The archive in which the file has been found (for info when performing
		/// multi-Archive searches, note you should still open through ResourceGroupManager)
		Archive* archive;
		/// The file's fully qualified name
		String filename;
		/// Path name; separated by '/' and ending with '/'
		String path;
		/// Base filename
		String basename;
		/// Compressed size
		size_t compressedSize;
		/// Uncompressed size
		size_t uncompressedSize;
	};

	typedef vector<FileInfo>::type FileInfoList;

	/**

	*/
	class LORD_CORE_API Archive : public ObjectAlloc
	{
	protected:
		/// Archive name
		String mName; 
		/// Archive type code
		String mType;

	public:
		/** Constructor - don't call direct, used by ArchiveFactory.
		*/
		Archive( const String& name, const String& archType )
			: mName(name), mType(archType) {}

		/** Default destructor.
		*/
		virtual ~Archive() {}

		/// Get the name of this archive
		const String& getName(void) const { return mName; }

		/// Returns whether this archive is case sensitive in the way it matches files
		virtual bool isCaseSensitive(void) const = 0;

		/** Loads the archive.
		@remarks
		This initializes all the internal data of the class.
		@warning
		Do not call this function directly, it is meant to be used
		only by the ArchiveManager class.
		*/
		virtual void load() = 0;

		/** Unloads the archive.
		@warning
		Do not call this function directly, it is meant to be used
		only by the ArchiveManager class.
		*/
		virtual void unload() = 0;

		/** Open a stream on a given file. 
		@note
		There is no equivalent 'close' method; the returned stream
		controls the lifecycle of this file operation.
		@param filename The fully qualified name of the file
		@param readOnly Whether to open the file in read-only mode or not (note, 
		if the archive is read-only then this cannot be set to false)
		@returns A shared pointer to a DataStream which can be used to 
		read / write the file. If the file is not present, returns a null
		shared pointer.
		*/
		virtual DataStream* open(const String& filename) = 0;

		// only ok in filesystem.
		virtual String location(const String& filename) { return StringUtil::BLANK; }

		/** List all file names in the archive.
		@note
		This method only returns filenames, you can also retrieve other
		information using listFileInfo.
		@param recursive Whether all paths of the archive are searched (if the 
		archive has a concept of that)
		@param dirs Set to true if you want the directories to be listed
		instead of files
		@returns A list of filenames matching the criteria, all are fully qualified
		*/
		virtual StringArray list(bool recursive = true, bool dirs = false) = 0;

		/** List all files in the archive with accompanying information.
		@param recursive Whether all paths of the archive are searched (if the 
		archive has a concept of that)
		@param dirs Set to true if you want the directories to be listed
		instead of files
		@returns A list of structures detailing quite a lot of information about
		all the files in the archive.
		*/
		virtual FileInfoList* listFileInfo(bool dirs = false) = 0;

		virtual StringArray find(const String& pattern, bool recursive = true,
			bool dirs = false) { return StringArray(); }
		/** Find out if the named file exists (note: fully qualified filename required) */
		virtual bool exists(const String& filename) = 0; 

		/// Return the type code of this Archive
		const String& getType(void) const { return mType; }

		// Return the unique resource name of a file in this archive
		virtual String getFileName(const String& innerName) const { return innerName; }

	};
}

#endif