#include "Core.h"
#include "ResourcePack.h"
#include "Object/Exception.h"
#include "ZipArchive.h"
#include "Object/Root.h"

namespace LORD
{

	//--------------------------------------------------------------------------
	ZipArchive::ZipArchive(const String& strName, const String& strArchType)
		:	Archive(strName, strArchType)
		,	mLoaded(false)
	{

	}
	//--------------------------------------------------------------------------
	ZipArchive::~ZipArchive()
	{
		unload();
	}
	//--------------------------------------------------------------------------
	bool ZipArchive::isCaseSensitive(void) const 
	{ 
		return false; 
	}
	//--------------------------------------------------------------------------
	void ZipArchive::load()
	{
		//EE_LOCK_MUTEX(mAutoMutex)
		String fullName = Root::Instance()->getRootPath() + mName;
		mResourcePack.OpenFile(fullName.c_str());
		mLoaded = true;
	}
	//--------------------------------------------------------------------------
	void ZipArchive::unload()
	{
		//EE_LOCK_MUTEX(mAutoMutex)
		if (mLoaded)
		{
			mResourcePack.CloseFile();
			mLoaded = false;
		}
	}
	//--------------------------------------------------------------------------
	DataStream* ZipArchive::open(const String& strFilename)
	{
		//EE_LOCK_MUTEX(mAutoMutex)
		DataStream* pResult = NULL;
		const unsigned char* cpbFileData	= NULL;
		unsigned char* pData		= NULL;
		unsigned int nOutLength		= 0;
		String strFile = strFilename;
		// 	if (!isCaseSensitive())
		// 	{
		// 		StringUtil::LowerCase(strFile);
		// 	}

		cpbFileData = mResourcePack.ReadFile(strFile.c_str(), nOutLength);
		if(!cpbFileData)
			LordException("Error: Error in function ZipArchive::open, read file %s fail!", strFile.c_str());

		pData = (unsigned char*)LORD_MALLOC(nOutLength); 

		memcpy(pData, cpbFileData, nOutLength);

		mResourcePack.ReleaseExtraMemory();

		pResult = LordNew ResourcePackDataStream(strFile, pData, nOutLength, true);
		return pResult;
	}
	//--------------------------------------------------------------------------
	StringArray ZipArchive::list(bool bRecursive /*= true*/, bool bDirs /*= false*/)
	{
		//EE_LOCK_MUTEX(mAutoMutex)
		const tFileNode* cpFileNodes	= NULL;
		int	nOutFileListCount			= 0;
		StringArray ret;
		
		cpFileNodes = mResourcePack.GetFileList(nOutFileListCount);
		if(!cpFileNodes)
			LordException("Error: Error in function ZipArchive::list, get file list fail!");

		for(int i = 0; i < nOutFileListCount; i++)
		{
			if(cpFileNodes[i].m_nUseFlag == FILE_NOT_USING)
			{
				break;
			}
			else if (cpFileNodes[i].m_nUseFlag == FILE_DELETE)
			{
				continue;
			}

			ret.push_back(cpFileNodes[i].m_sFileName);
		}
		return ret;
	}
	//--------------------------------------------------------------------------
	FileInfoList* ZipArchive::listFileInfo(bool bDirs /*= false*/)
	{
		//EE_LOCK_MUTEX(mAutoMutex)
		const tFileNode* cpFileNodes	= NULL;
		int	nOutFileListCount			= 0;
		FileInfoList* pResult			= NULL;
		FileInfo fileInfo;

		cpFileNodes = mResourcePack.GetFileList(nOutFileListCount);
		if(!cpFileNodes)
			LordException("Error: Error in function ZipArchive::list, get file list fail!");

		pResult = LordNewT(FileInfoList);

		for(int i = 0; i < nOutFileListCount; i++)
		{
			if(cpFileNodes[i].m_nUseFlag == FILE_NOT_USING)
			{
				break;
			}
			else if (cpFileNodes[i].m_nUseFlag == FILE_DELETE)
			{
				continue;
			}
			fileInfo.archive  = this;
			fileInfo.filename = cpFileNodes[i].m_sFileName;	
			fileInfo.basename = cpFileNodes[i].m_sFileName;	
			fileInfo.path	  = "";
			fileInfo.compressedSize = cpFileNodes[i].m_nFileCompressSize;
			fileInfo.uncompressedSize = cpFileNodes[i].m_nFileRealSize;

			pResult->push_back(fileInfo);
		}
		return pResult;
	}
	//--------------------------------------------------------------------------
	bool ZipArchive::exists(const String& strFilename)
	{
		//EE_LOCK_MUTEX(mAutoMutex)
		return mResourcePack.IsFileExist(strFilename.c_str());
	}
}
