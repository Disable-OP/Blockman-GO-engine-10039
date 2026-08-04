#include "Core.h"
#include "FileSystem.h"
#include "Object/Exception.h"
#include "Object/Root.h"
#include "Util/PathUtil.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#if (LORD_PLATFORM != LORD_PLATFORM_WINDOWS)

#include <dirent.h>
#include <unistd.h>
#include <fnmatch.h>

/* Our simplified data entry structure */
struct _finddata_t
{
	char *name;
	int attrib;
	unsigned long size;
};

#define _A_NORMAL 0x00  /* Normalfile-Noread/writerestrictions */
#define _A_RDONLY 0x01  /* Read only file */
#define _A_HIDDEN 0x02  /* Hidden file */
#define _A_SYSTEM 0x04  /* System file */
#define _A_ARCH   0x20  /* Archive file */
#define _A_SUBDIR 0x10  /* Subdirectory */

intptr_t _findfirst(const char *pattern, struct _finddata_t *data);
int _findnext(intptr_t id, struct _finddata_t *data);
int _findclose(intptr_t id);

struct _find_search_t
{
	char *pattern;
	char *curfn;
	char *directory;
	int dirlen;
	DIR *dirfd;
};

intptr_t _findfirst(const char *pattern, struct _finddata_t *data)
{
	_find_search_t *fs = new _find_search_t;
	fs->curfn = NULL;
	fs->pattern = NULL;

	// Separate the mask from directory name
	const char *mask = strrchr (pattern, '/');
	if (mask)
	{
		fs->dirlen = mask - pattern;
		mask++;
		fs->directory = (char *)malloc (fs->dirlen + 1);
		memcpy (fs->directory, pattern, fs->dirlen);
		fs->directory [fs->dirlen] = 0;
	}
	else
	{
		mask = pattern;
		fs->directory = strdup (".");
		fs->dirlen = 1;
	}

	fs->dirfd = opendir (fs->directory);
	if (!fs->dirfd)
	{
		_findclose ((intptr_t)fs);
		return -1;
	}

	/* Hack for "*.*" -> "*' from DOS/Windows */
	if (strcmp (mask, "*.*") == 0)
		mask += 2;
	fs->pattern = strdup (mask);

	/* Get the first entry */
	if (_findnext ((intptr_t)fs, data) < 0)
	{
		_findclose ((intptr_t)fs);
		return -1;
	}

	return (intptr_t)fs;
}

int _findnext(intptr_t id, struct _finddata_t *data)
{
	_find_search_t *fs = (_find_search_t *)id;

	/* Loop until we run out of entries or find the next one */
	dirent *entry;
	for (;;)
	{
		if (!(entry = readdir (fs->dirfd)))
			return -1;

		/* See if the filename matches our pattern */
		if (fnmatch (fs->pattern, entry->d_name, 0) == 0)
			break;
	}

	if (fs->curfn)
		free (fs->curfn);
	data->name = fs->curfn = strdup (entry->d_name);

	size_t namelen = strlen (entry->d_name);
	char *xfn = new char [fs->dirlen + 1 + namelen + 1];
	sprintf (xfn, "%s/%s", fs->directory, entry->d_name);

	/* stat the file to get if it's a subdir and to find its length */
	struct stat stat_buf;
	if (stat (xfn, &stat_buf))
	{
		// Hmm strange, imitate a zero-length file then
		data->attrib = _A_NORMAL;
		data->size = 0;
	}
	else
	{
		if (S_ISDIR(stat_buf.st_mode))
			data->attrib = _A_SUBDIR;
		else
			/* Default type to a normal file */
			data->attrib = _A_NORMAL;

		data->size = (unsigned long)stat_buf.st_size;
	}

	delete [] xfn;

	/* Files starting with a dot are hidden files in Unix */
	if (data->name [0] == '.')
		data->attrib |= _A_HIDDEN;

	return 0;
}

int _findclose(intptr_t id)
{
	int ret;
	_find_search_t *fs = (_find_search_t *)id;

	ret = fs->dirfd ? closedir (fs->dirfd) : 0;
	free (fs->pattern);
	free (fs->directory);
	if (fs->curfn)
		free (fs->curfn);
	delete fs;

	return ret;
}

#endif

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <Windows.h>
#  include <direct.h>
#  include <io.h>
#endif


namespace LORD
{
	//-----------------------------------------------------------------------
	static bool is_reserved_dir(const char *fn)
	{
		return (fn[0] == '.' && (fn[1] == 0 || (fn[1] == '.' && fn[2] == 0)));
	}
	//-----------------------------------------------------------------------
	FileSystemArchive::FileSystemArchive(const String& name, const String& archType )
		: Archive(name, archType)
	{
	}
	//-----------------------------------------------------------------------
	bool FileSystemArchive::isCaseSensitive(void) const
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return false;
#else
		return false;
#endif

	}
	//-----------------------------------------------------------------------
	static bool is_absolute_path(const char* path)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		if (isalpha(ui8(path[0])) && path[1] == ':')
			return true;
#endif
		return path[0] == '/' || path[0] == '\\';
	}
	//-----------------------------------------------------------------------
	/*
	static bool is_reserved_dir (const char *fn)
	{
	return (fn [0] == '.' && (fn [1] == 0 || (fn [1] == '.' && fn [2] == 0)));
	}
	*/
	//-----------------------------------------------------------------------
	static String concatenate_path(const String& base, const String& name)
	{
		if (base.empty() || is_absolute_path(name.c_str()))
			return name;
		else
			return base + '/' + name;
	}
	//-----------------------------------------------------------------------
	/*
	void FileSystemArchive::findFiles(const String& pattern, bool dirs, StringArray *simpleList, FileInfoList* detailList)
	{
	long lHandle, res;
	struct _finddata_t tagData;

	// pattern can contain a directory name, separate it from mask
	size_t pos1 = pattern.rfind ('/');
	size_t pos2 = pattern.rfind ('\\');
	if (pos1 == pattern.npos || ((pos2 != pattern.npos) && (pos1 < pos2)))
	pos1 = pos2;
	String directory;
	if (pos1 != pattern.npos)
	directory = pattern.substr (0, pos1 + 1);

	String full_pattern = concatenate_path(mName, pattern);

	lHandle = _findfirst(full_pattern.c_str(), &tagData);
	res = 0;
	while (lHandle != -1 && res != -1)
	{
	if ((dirs == ((tagData.attrib & _A_SUBDIR) != 0)) &&
	( (tagData.attrib & _A_HIDDEN) == 0 ) &&
	(!dirs || !is_reserved_dir (tagData.name)))
	{
	if (simpleList)
	{
	simpleList->push_back(directory + tagData.name);
	}
	else if (detailList)
	{
	FileInfo fi;
	fi.archive = this;
	fi.filename = directory + tagData.name;
	fi.basename = tagData.name;
	fi.path = directory;
	fi.compressedSize = tagData.size;
	fi.uncompressedSize = tagData.size;
	detailList->push_back(fi);
	}
	}
	res = _findnext( lHandle, &tagData );
	}
	// Close if we found any files
	if(lHandle != -1)
	_findclose(lHandle);
	}
	*/
	//-----------------------------------------------------------------------
	FileSystemArchive::~FileSystemArchive()
	{
		unload();
	}
	//-----------------------------------------------------------------------
	void FileSystemArchive::load()
	{
	}
	//-----------------------------------------------------------------------
	void FileSystemArchive::unload()
	{
	}
	//-----------------------------------------------------------------------
	String FileSystemArchive::location(const String& filename)
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		return concatenate_path(mName, filename);
#else
		String full_path = concatenate_path(mName, filename);
		if (StringUtil::EndWith(mName, "/minimap") || StringUtil::EndWith(mName, "/dynamic") || StringUtil::EndWith(mName, "/lang"))
		{
			return Root::Instance()->getMapPath() + full_path.c_str();
		}
		return Root::Instance()->getRootPath() + full_path.c_str();
#endif
	}
	//-----------------------------------------------------------------------
	DataStream* FileSystemArchive::open(const String& filename)
	{
		String full_path = concatenate_path(mName, filename);

		// Use filesystem to determine size 
		// (quicker than streaming to the end and back)
		struct stat tagStat;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		String fullPath = full_path.c_str();
#else
		String fullPath = Root::Instance()->getRootPath() + full_path.c_str();
		if (StringUtil::EndWith(mName, "/minimap") || StringUtil::EndWith(mName, "/dynamic") || StringUtil::EndWith(mName, "/lang"))
		{
			fullPath = Root::Instance()->getMapPath() + full_path.c_str();
		}
#endif
		int ret = stat(fullPath.c_str(), &tagStat);

		if(ret != 0)
		{
            LordLogError("FileSystemArchive error  code %d    msg %s  fullPath %s"  , errno, strerror(errno) , fullPath.c_str());
			return nullptr;
		}

		(void)ret;  // Silence warning

		std::ifstream* origStream = LordNewT(std::ifstream);
		origStream->open(fullPath.c_str(), std::ios::in | std::ios::binary);

		if(origStream->fail())
		{
            //std::cerr << "Error: " << std::strerror(errno);
			LordDeleteT(origStream, basic_ifstream);
			LordException("Error: Cannot open file: %s in FileSystemArchive::open", filename.c_str());
		}
		FileStreamDataStream* stream = LordNew FileStreamDataStream(filename, 
			origStream, tagStat.st_size, true);

		return stream;
	}


	//-----------------------------------------------------------------------
	StringArray FileSystemArchive::list(bool recursive, bool dirs)
	{
		StringArray strs;
		String fullPath = Root::Instance()->getRootPath() + mName;

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
		if (StringUtil::EndWith(mName, "/minimap") || StringUtil::EndWith(mName, "/dynamic") || StringUtil::EndWith(mName, "/lang"))
		{
			fullPath = Root::Instance()->getMapPath() + mName;
		}
#endif
		PathUtil::EnumFilesInDir(strs, fullPath, dirs, false, false);

		//findFiles("*", dirs, ret, 0);

		return strs;
	}
	//-----------------------------------------------------------------------
	FileInfoList* FileSystemArchive::listFileInfo(bool dirs)
	{
		FileInfoList* ret = LordNewT(FileInfoList);

		//findFiles("*", dirs, 0, ret);

		return ret;
	}
	//-----------------------------------------------------------------------
	LORD::StringArray FileSystemArchive::find(const String& pattern, bool recursive /*= true*/, bool dirs /*= false*/)
	{
		// Note that we have to tell the SharedPtr to use ECHO_DELETE_T not ECHO_DELETE by passing category
		StringArray ret;

		findFiles(pattern, dirs, &ret, 0);

		return ret;
	}
	//-----------------------------------------------------------------------
	bool FileSystemArchive::exists(const String& filename)
	{
		String full_path = concatenate_path(mName, filename);

		struct stat tagStat;
		bool ret = (stat(full_path.c_str(), &tagStat) == 0);

		// stat will return true if the filename is absolute, but we need to check
		// the file is actually in this archive
		if (ret && is_absolute_path(filename.c_str()))
		{
			// only valid if full path starts with our base
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			// case insensitive on windows
			String lowerCaseName = mName;
			StringUtil::LowerCase(lowerCaseName);
			ret = StringUtil::StartWith(full_path, lowerCaseName, true);
#else
			// case sensitive
			ret = StringUtil::StartWith(full_path, mName);
#endif
		}

		return ret;
	}

	void FileSystemArchive::findFiles(const String& pattern, bool dirs, StringArray* simpleList, FileInfoList* detailList)
	{
		long lHandle, res;
		struct _finddata_t tagData;

		// pattern can contain a directory name, separate it from mask
		size_t pos1 = pattern.rfind('/');
		size_t pos2 = pattern.rfind('\\');
		if (pos1 == pattern.npos || ((pos2 != pattern.npos) && (pos1 < pos2)))
			pos1 = pos2;
		String directory;
		if (pos1 != pattern.npos)
			directory = pattern.substr(0, pos1 + 1);

		String full_pattern = concatenate_path(mName, pattern);

		lHandle = _findfirst(full_pattern.c_str(), &tagData);
		res = 0;
		while (lHandle != -1 && res != -1)
		{
			if ((dirs == ((tagData.attrib & _A_SUBDIR) != 0)) &&
				((tagData.attrib & _A_HIDDEN) == 0) &&
				(!dirs || !is_reserved_dir(tagData.name)))
			{
				if (simpleList)
				{
					simpleList->push_back(directory + tagData.name);
				}
				else if (detailList)
				{
					FileInfo fi;
					fi.archive = this;
					fi.filename = directory + tagData.name;
					fi.basename = tagData.name;
					fi.path = directory;
					fi.compressedSize = tagData.size;
					fi.uncompressedSize = tagData.size;
					detailList->push_back(fi);
				}
			}
			res = _findnext(lHandle, &tagData);
		}
		// Close if we found any files
		if (lHandle != -1)
			_findclose(lHandle);
	}

	//-----------------------------------------------------------------------
	const String& FileSystemArchiveFactory::getType(void) const
	{
		static String name = "FileSystem";
		return name;
	}

}
