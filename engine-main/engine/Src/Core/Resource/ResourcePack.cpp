#include "Core.h"
#include "ResourcePack.h"
#include "Util/PathUtil.h"
#include "zlib.h"
#include "Util/AssertX.h"

namespace LORD
{
	static void* Zlib_User_Alloc(void* opaque,unsigned int items,unsigned int size)
	{
		return LordMalloc(size*items);
	}

	static void Zlib_User_Free(void* opaque,void* ptr)
	{
		LordFree(ptr);
	}

	int ResourcePack::Zlib_UnCompress(unsigned char* dest,unsigned int* destLen,const unsigned char* source,unsigned int sourceLen)
	{
		z_stream stream;
		int err;

		stream.next_in = (Bytef*)source;
		stream.avail_in = (unsigned int)sourceLen;
		/* Check for source > 64K on 16-bit machine: */
		if (stream.avail_in != sourceLen) return Z_BUF_ERROR;

		stream.next_out = dest;
		stream.avail_out = *destLen;
		if (stream.avail_out != *destLen) return Z_BUF_ERROR;

		stream.zalloc = &Zlib_User_Alloc;  //0;
		stream.zfree  = &Zlib_User_Free;   //0;

		err = inflateInit(&stream);
		if (err != Z_OK) return err;

		err = inflate(&stream, Z_FINISH);
		if (err != Z_STREAM_END) {
			inflateEnd(&stream);
			return err == Z_OK ? Z_BUF_ERROR : err;
		}
		*destLen = stream.total_out;

		err = inflateEnd(&stream);
		return err;
	}

	int	ResourcePack::Zlib_Compress(unsigned char* dest,unsigned int* destLen,const unsigned char* source,unsigned int sourceLen)
	{
		z_stream stream;
		int err;

		stream.next_in = (Bytef*)source;
		stream.avail_in = (unsigned int)sourceLen;
		stream.next_out = dest;
		stream.avail_out = (unsigned int)*destLen;
		if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

		stream.zalloc = &Zlib_User_Alloc;	//0;
		stream.zfree  = &Zlib_User_Free;		//0;
		stream.opaque = (void*)0;

		err = deflateInit(&stream, Z_BEST_SPEED);
		if (err != Z_OK) return err;

		err = deflate(&stream, Z_FINISH);
		if (err != Z_STREAM_END) {
			deflateEnd(&stream);
			return err == Z_OK ? Z_BUF_ERROR : err;
		}
		*destLen = stream.total_out;

		err = deflateEnd(&stream);
		return err;
	}

	ResourcePack::ResourcePack()
	{
		m_nFileListOffset		= 0;
		m_nFilesListNum			= MIN_FILE_NODE_NUM;
		m_pFileNode				= NULL;
		m_pArchiveFile			= NULL;
		m_nCurrFileOffset		= 0;
		m_bArchiveFileChange	= false;
		m_sErrorString			= "";
		memset(m_sArchiveFileName, 0, sizeof(m_sArchiveFileName));
		memset(m_sFileVersion, 0, sizeof(m_sFileVersion));

		m_RealFileBuffer = NULL;
		m_RealFileBufferSize = 0;
		m_CompressBuffer = NULL;
		m_CompressBufferSize = 0;
		m_UnCompressBuffer = NULL;
		m_UnCompressBufferSize = 0;
		m_OutBuffer = NULL;
		m_OutBufferSize = 0;
	}

	void ResourcePack::FreeMemory()
	{
		if(m_RealFileBuffer)
		{
			LordFree(m_RealFileBuffer);
			m_RealFileBuffer = NULL;
			m_RealFileBufferSize = 0;
		}

		if(m_CompressBuffer)
		{
			LordFree(m_CompressBuffer);
			m_CompressBuffer = NULL;
			m_CompressBufferSize = 0;
		}

		if(m_UnCompressBuffer)
		{
			LordFree(m_UnCompressBuffer);
			m_UnCompressBuffer = NULL;
			m_UnCompressBufferSize = 0;
		}

		if(m_OutBuffer)
		{
			LordFree(m_OutBuffer);
			m_OutBuffer = NULL;
			m_OutBufferSize = 0;
		}
	}

	ResourcePack::~ResourcePack()
	{
		if(m_pArchiveFile)
		{
			fclose(m_pArchiveFile);
			m_pArchiveFile = NULL;
		}

		if(m_pFileNode)
			delete []m_pFileNode;

		FreeMemory();
	}

	void ResourcePack::ResetMember()
	{
		m_nFileListOffset		= 0;
		m_nFilesListNum			= MIN_FILE_NODE_NUM;
		if(m_pFileNode)
			delete []m_pFileNode;

		m_pFileNode				= NULL;
		m_pArchiveFile			= NULL;
		m_nCurrFileOffset		= 0;
		m_bArchiveFileChange	= false;
		m_sErrorString			= "";
		memset(m_sArchiveFileName, 0, sizeof(m_sArchiveFileName));
		memset(m_sFileVersion, 0, sizeof(m_sFileVersion));
		FreeMemory();
		m_FileNodeMap.clear();
	}
	
	bool ResourcePack::CreateFile(const char *lpszArchiveFile, int nFileNodeNum)
	{
		if(!lpszArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::CreateFile, lpszArchiveFile is NULL!\n";
			return false;
		}
		if(m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::CreateFile, m_pArchiveFile is not NULL!\n";
			fclose(m_pArchiveFile);
			m_pArchiveFile = NULL;
			return false;
		}
		m_pArchiveFile = fopen(lpszArchiveFile, "rb");
		if(m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::CreateFile, archive exist!\n";
			fclose(m_pArchiveFile);
			m_pArchiveFile = NULL;
			return false;
		}
		strncpy(m_sArchiveFileName, lpszArchiveFile, sizeof(m_sArchiveFileName));

		m_pArchiveFile = fopen(lpszArchiveFile, "w+b");
		if(!m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::CreateFile, open archive file error!\n";
			return false;
		}

		if(nFileNodeNum>=MIN_FILE_NODE_NUM && nFileNodeNum<=MAX_FILE_NODE_NUM)
			m_nFilesListNum = nFileNodeNum;
		if(m_nFilesListNum<MIN_FILE_NODE_NUM)
			m_nFilesListNum = MIN_FILE_NODE_NUM;
		else if(m_nFilesListNum>MAX_FILE_NODE_NUM)
			m_nFilesListNum = MAX_FILE_NODE_NUM;

		m_nFileListOffset = sizeof(m_nFileListOffset) + sizeof(m_nFilesListNum) + sizeof(m_sFileVersion);

		fwrite(&m_nFileListOffset, sizeof(m_nFileListOffset), 1, m_pArchiveFile);
		fwrite(&m_nFilesListNum, sizeof(m_nFilesListNum), 1, m_pArchiveFile);
		fwrite(m_sFileVersion, sizeof(m_sFileVersion), 1, m_pArchiveFile);

		m_pFileNode = new tFileNode[m_nFilesListNum];
		if(!m_pFileNode)
		{
			m_sErrorString = "Error in function CLizArchive::CreateFile, create archive file list error!\n";
			return false;
		}

		for(int i=0; i<m_nFilesListNum; i++)
		{
			fwrite(&m_pFileNode[i], sizeof(tFileNode), 1, m_pArchiveFile);
		}

		fclose(m_pArchiveFile);
		m_pArchiveFile = NULL;
		delete []m_pFileNode;
		m_pFileNode = NULL;
		return true;
	}


	bool ResourcePack::OpenFile(const char* lpszArchiveFile)
	{
		LordAssert(lpszArchiveFile);
		LordAssert(!m_pArchiveFile);

		char tmpArchiveFile[256] = {0};
		tmpArchiveFile[255] = '\0';
		m_FileNodeMap.clear();

		EE_LOCK_MUTEX(m_UnpressLock);

		strncpy(tmpArchiveFile, lpszArchiveFile, sizeof(tmpArchiveFile));

		ResetMember();
		m_pArchiveFile = fopen(tmpArchiveFile, "r+b");
		LordAssert(m_pArchiveFile);

		strncpy(m_sArchiveFileName, tmpArchiveFile, sizeof(m_sArchiveFileName));

		fread(&m_nFileListOffset, sizeof(m_nFileListOffset), 1, m_pArchiveFile);
		fread(&m_nFilesListNum, sizeof(m_nFilesListNum), 1, m_pArchiveFile);
		fread(m_sFileVersion, sizeof(m_sFileVersion), 1, m_pArchiveFile);

		LordAssert(m_nFileListOffset!=0);
		LordAssert(m_nFilesListNum!=0);
		LordAssert(m_nFilesListNum>=MIN_FILE_NODE_NUM);
		LordAssert(m_nFilesListNum<=MAX_FILE_NODE_NUM);
	
		if(m_pFileNode)
			delete []m_pFileNode;

		m_pFileNode = new tFileNode[m_nFilesListNum];

		fseek(m_pArchiveFile, m_nFileListOffset, SEEK_SET);
		for(int i=0; i<m_nFilesListNum; i++)
		{
			fread(&m_pFileNode[i], sizeof(tFileNode), 1, m_pArchiveFile);
			if(m_pFileNode[i].m_sFileName[0] != 0 && m_pFileNode[i].m_nUseFlag == FILE_IN_USING)
			{
				String temStr = m_pFileNode[i].m_sFileName;
				StringUtil::LowerCase(temStr);
				m_FileNodeMap.insert(std::pair<String, int>(temStr, i));
			}
		}

		return true;
	}

	bool ResourcePack::AddFile(const char *lpszFilePath, const char *lpszFileName, int nCompressType)
	{
		if(!lpszFilePath || !lpszFileName)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, lpszFilePath or lpszFileName is NULL!\n";
			return false;
		}

		if(!m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, archive file is closed!\n";
			return false;
		}

		if(!m_pFileNode)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, file list error!\n";
			return true;
		}

		if(IsFileExist(lpszFileName))
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, the file added is exist!\n";
			printf("Press any key to Continue");
			getc(stdin);
			return true;
		}

		int nEmptyFilePos = GetEmptyFileNode();
		if(nEmptyFilePos==-1)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, empty file's position is error!\n";
			return false;
		}
		else if(nEmptyFilePos==-2)
		{
			if(!RebuildPacket())
			{
				m_sErrorString = "Error in function CLizArchive::AddFile, rebuild packet error!\n";
				return false;
			}
			return AddFile(lpszFilePath, lpszFileName, nCompressType);
		}

		if(nCompressType == -1)
		{
			m_pFileNode[nEmptyFilePos].m_nCompressType = (rand()%2);
		}
		else
		{
			m_pFileNode[nEmptyFilePos].m_nCompressType = nCompressType;
		}
		strncpy(m_pFileNode[nEmptyFilePos].m_sFileName, lpszFileName, sizeof(m_pFileNode[nEmptyFilePos].m_sFileName));

		unsigned int RealSize = 0;
		if(!CompressData (m_pFileNode[nEmptyFilePos], lpszFilePath, RealSize))
			return false;

		m_bArchiveFileChange = true;
		return true;
	}

	bool ResourcePack::AddZlibFile(const char *lpszFileName)
	{
		const unsigned char	*lpszOut = NULL;
		unsigned int		nFileRealSize = 0;
		unsigned int        compressSize = 0;

		String str = StringUtil::Format("%s%s", m_sArchiveFilePath.c_str(), lpszFileName);
		FILE *fp = fopen(str.c_str(), "rb");
		if (!fp)
		{
			m_sErrorString = "Error in function CLizArchive::CompressData, the file added is error!\n";
			return false;
		}
		fseek(fp, 0L, SEEK_END);
		nFileRealSize = ftell(fp);
		if (nFileRealSize > m_RealFileBufferSize)
		{
			m_RealFileBufferSize = nFileRealSize;
			if (m_RealFileBuffer)
			{
				LordFree(m_RealFileBuffer);
			}
			m_RealFileBuffer = (unsigned char*)LordMalloc(m_RealFileBufferSize);
		}

		fseek(fp, 0L, SEEK_SET);
		fread(m_RealFileBuffer, nFileRealSize, 1, fp);
		fclose(fp);

		tFileNode node;
		node.m_nFileRealSize = ((unsigned int*)m_RealFileBuffer)[0];
		node.m_nCompressType = CT_ZLIB;

		return AddFile(lpszFileName, &node, (char*)(m_RealFileBuffer + sizeof(unsigned int)), nFileRealSize-sizeof(unsigned int));
	}

	bool ResourcePack::AddFile(const char *lpszFileName, const tFileNode* pFileNode, const char *pData, int nSize)
	{
		int					nEmptyFilePos	= -1;
		int					nFileRealSize	= 0;
		//unsigned long		nCompressSizeHash	= 0;

		if(!lpszFileName)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, lpszFileName is NULL!\n";
			return false;
		}

		if(!m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, archive file is closed!\n";
			return false;
		}

		if(!m_pFileNode)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, file list error!\n";
			return true;
		}

		if (IsFileExist(lpszFileName))
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, the file added is exist!\n";
			printf("Press any key to continue");
			getc(stdin);
			return true;
		}

		nEmptyFilePos = GetEmptyFileNode();
		if(nEmptyFilePos==-1)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, empty file's position is error!\n";
			return false;
		}
		else if(nEmptyFilePos==-2)
		{
			if(!RebuildPacket())
			{
				m_sErrorString = "Error in function CLizArchive::AddFile, rebuild packet error!\n";
				return false;
			}
			return AddFile(lpszFileName, pFileNode, pData, nSize);
		}

		nFileRealSize = pFileNode->m_nFileRealSize;

		if(nSize<=0)
		{
			m_sErrorString = "Error in function CLizArchive::AddFile, compress file error!\n";
			//getc(stdin);
			return true;
		}

		fseek(m_pArchiveFile, 0L, SEEK_END);
		m_nCurrFileOffset = ftell(m_pArchiveFile);
		fseek(m_pArchiveFile, 0, SEEK_END);

		fwrite(pData, nSize, 1, m_pArchiveFile);


		strncpy(m_pFileNode[nEmptyFilePos].m_sFileName, lpszFileName, sizeof(m_pFileNode[nEmptyFilePos].m_sFileName));
		m_pFileNode[nEmptyFilePos].m_nUseFlag = FILE_IN_USING;
		m_pFileNode[nEmptyFilePos].m_nOffset = m_nCurrFileOffset;
		m_pFileNode[nEmptyFilePos].m_nFileRealSize = nFileRealSize;
		m_pFileNode[nEmptyFilePos].m_nFileCompressSize = nSize;
		m_pFileNode[nEmptyFilePos].m_nCompressType = pFileNode->m_nCompressType;

		m_bArchiveFileChange = true;
		return true;
	}


	bool ResourcePack::DelFile(const char *lpszFileName)
	{
		int	nFileNodePos	= 0;
		if(!lpszFileName)
		{
			m_sErrorString = "Error in function CLizArchive::DelFile, lpszFileName is NULL!\n";
			return false;
		}

		if(!m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::DelFile, archive file is closed!\n";
			return false;
		}

		if(!m_pFileNode)
		{
			m_sErrorString = "Error in function CLizArchive::DelFile, file list error!\n";
			return true;
		}

		nFileNodePos = GetFileNodeByName(lpszFileName);
		if(nFileNodePos<0)
		{
			m_sErrorString = "Error in function CLizArchive::DelFile, the file deleted is not existed!\n";
			return false;
		}

		m_pFileNode[nFileNodePos].m_nUseFlag = FILE_DELETE;

		m_bArchiveFileChange = true;
		return true;
	}

	bool ResourcePack::SetVersion(const char* szVersion)
	{
		if(!m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::SetVersion, archive file is closed!\n";
			return false;
		}

		memset(m_sFileVersion, 0, sizeof(m_sFileVersion));
		strcpy(m_sFileVersion, szVersion);
		fseek(m_pArchiveFile, sizeof(m_nFileListOffset)+sizeof(m_nFilesListNum), SEEK_SET);
		fwrite(m_sFileVersion, sizeof(m_sFileVersion), 1, m_pArchiveFile);

		return true;
	}

	bool ResourcePack::CloseFile()
	{
		LordAssert(m_pArchiveFile);
		
		if(m_bArchiveFileChange)
		{
			fseek(m_pArchiveFile, m_nFileListOffset, SEEK_SET);
			for(int i=0; i<m_nFilesListNum; i++)
			{
				fwrite(&m_pFileNode[i], sizeof(tFileNode), 1, m_pArchiveFile);
			}
		}
		
		fclose(m_pArchiveFile);
		m_pArchiveFile = NULL;
		return true;
	}

	const char* ResourcePack::GetVersion()
	{
		LordAssert(m_pArchiveFile);

		return m_sFileVersion;
	}

	const unsigned char* ResourcePack::ReadFile(const char* lpszFileName, unsigned int& nOutLength)
	{
		int	nFileNodePos	= 0;
		//const unsigned char* lpszOut		= NULL;
		//char*		lpszCompressOut	= NULL;

		LordAssert(lpszFileName);
		LordAssert(m_pArchiveFile);
		LordAssert(m_pFileNode);

		nFileNodePos = GetFileNodeByName_fast(lpszFileName);
		if(nFileNodePos<0)
		{
			m_sErrorString = "Error in function CLizArchive::ReadFile, the file deleted is not existed!\n";
			return NULL;
		}

		m_UnpressLock.Lock();
		if(UnCompressData (m_pFileNode[nFileNodePos], nOutLength))
		{
			m_UnpressLock.Unlock();
			return m_OutBuffer;
		}
		m_UnpressLock.Unlock();
		nOutLength = 0;
		return NULL;
	}

	bool ResourcePack::RebuildPacket()
	{
		ResourcePack rebuildPacket;
		rebuildPacket.ResetMember();

		String szRebuildPacketName;
		int nFileListNum = 0;

		szRebuildPacketName = StringUtil::Format("%s.new", m_sArchiveFileName);
		if(m_nFilesListNum*2 > 10000)
			nFileListNum = m_nFilesListNum + 2000;
		else
			nFileListNum = m_nFilesListNum * 2;


        if(PathUtil::IsFileExist(szRebuildPacketName))
        {
            // 如果存在就删除
            PathUtil::DelPath(szRebuildPacketName);
        }
        
		if(!rebuildPacket.CreateFile(szRebuildPacketName.c_str(), nFileListNum))
		{
			m_sErrorString = "Error in function CLizArchive::RebuildPacket, rebuildPacket create file error!\n";
			return false;
		}

		if(!rebuildPacket.OpenFile(szRebuildPacketName.c_str()))
		{
			m_sErrorString = "Error in function CLizArchive::RebuildPacket, rebuildPacket open file error!\n";
			return false;
		}

		rebuildPacket.SetVersion(GetVersion());

		if(!rebuildPacket.CopyFileList(m_pFileNode, m_nFilesListNum))
		{
			m_sErrorString = "Error in function CLizArchive::RebuildPacket, rebuildPacket copy file list error!\n";
			return false;
		}

		CloseFile();

		if(!rebuildPacket.CopyFileCompressData(m_sArchiveFileName, m_nFileListOffset+m_nFilesListNum*sizeof(tFileNode)))
		{
			m_sErrorString = "Error in function CLizArchive::RebuildPacket, rebuildPacket copy file compress data error!\n";
			return false;
		}

		rebuildPacket.CloseFile();

		String strFilename = m_sArchiveFileName;
		if(!PathUtil::DelPath(strFilename))
		{
			m_sErrorString = "Error in function CLizArchive::RebuildPacket, delete old packet file error!\n";
			return false;
		}

		String strPacketName = szRebuildPacketName;
		String strArchiveName = m_sArchiveFileName;
		if(!PathUtil::RenameFile(strPacketName, strArchiveName))
		{
			m_sErrorString = "Error in function CLizArchive::RebuildPacket, rename rebuild packet file error!\n";
			return false;
		}

		if(!OpenFile(m_sArchiveFileName))
		{
			m_sErrorString = "Error in function CLizArchive::RebuildPacket, open rebuild packet file error!\n";
			return false;
		}

		return true;
	}

	int ResourcePack::GetEmptyFileNode()
	{
		if(!m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::GetEmptyFileNode, archive file is closed!\n";
			return -1;
		}

		if(!m_pFileNode)
		{
			m_sErrorString = "Error in function CLizArchive::GetEmptyFileNode, file list error!\n";
			return -1;
		}

		for(int i=0; i<m_nFilesListNum; i++)
		{
			if(m_pFileNode[i].m_nUseFlag == FILE_NOT_USING)
				return i;
		}

		return -2;
	}

	bool ResourcePack::SetArchivePath( const char* apszArchivePath )
	{
		if ( NULL == apszArchivePath )
		{
			return false;
		}

		m_sArchiveFilePath = StringUtil::Format("%s/", apszArchivePath );

		return true;
	}

	bool ResourcePack::GetFileCompressData( const char* aszFileName, char* aszOutPut )
	{
		if ( NULL == aszFileName )
		{
			m_sErrorString = "Error in function CLizArchive::GetFileCompressData, aszFileName is NULL!\n";
			return false;
		}

		if ( NULL == aszOutPut )
		{
			m_sErrorString = "Error in function CLizArchive::GetFileCompressData, aszOutPut is NULL!\n";
			return false;
		}

		if(!m_pArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::GetFileCompressData, archive file is closed!\n";
			return false;
		}

		if(!m_pFileNode)
		{
			m_sErrorString = "Error in function CLizArchive::GetFileCompressData, file list error!\n";
			return true;
		}

		if(!IsFileExist(aszFileName))
		{
			m_sErrorString = "Error in function CLizArchive::GetFileCompressData, the file added is not exist!\n";
			printf("Press any key to Continue");
			getc(stdin);
			return true;
		}

		int liFilePos = GetFileNodeByName( aszFileName );
		if ( liFilePos < 0 )
		{
			return false;
		}
		fseek( m_pArchiveFile, m_pFileNode[liFilePos].m_nOffset, SEEK_SET );
		fread( aszOutPut, sizeof( char ), m_pFileNode[liFilePos].m_nFileCompressSize, m_pArchiveFile );

		return true;
	}


	bool ResourcePack::CompressData(tFileNode &fileNode, const char* filePath, unsigned int &nCompressSize)
	{
		const unsigned char	*lpszOut		= NULL;
		unsigned int		nFileRealSize	= 0;
		FILE				*fp				= NULL;
		//unsigned long		nCompressSizeHash	= 0;
		unsigned int        compressSize	= 0;

		String str = StringUtil::Format("%s%s", m_sArchiveFilePath.c_str(), fileNode.m_sFileName);
		fp = fopen(/*filePath*/str.c_str(), "rb");
		if(!fp)
		{
			m_sErrorString = "Error in function CLizArchive::CompressData, the file added is error!\n";
			return false;
		}
		fseek(fp, 0L, SEEK_END);
		nFileRealSize = ftell(fp);
		if(nFileRealSize > m_RealFileBufferSize)
		{
			m_RealFileBufferSize = nFileRealSize;
			if(m_RealFileBuffer)
			{
				LordFree(m_RealFileBuffer);
			}
			m_RealFileBuffer = (unsigned char*)LordMalloc(m_RealFileBufferSize);
		}

		fseek(fp, 0L, SEEK_SET);
		fread(m_RealFileBuffer, nFileRealSize, 1, fp);
		fclose(fp);

		switch(fileNode.m_nCompressType)
		{
		case CT_NONE:
			lpszOut = m_RealFileBuffer;
			nCompressSize = nFileRealSize;
			break;
		case CT_ZLIB:
			if(nFileRealSize*2 > m_CompressBufferSize)
			{
				if(m_CompressBuffer)
				{
					LordFree(m_CompressBuffer);
				}
				m_CompressBufferSize = (nFileRealSize*2)>(MAX_MEMORY_CACHE)?(nFileRealSize*2):(MAX_MEMORY_CACHE);
				m_CompressBuffer = (unsigned char*)LordMalloc(m_CompressBufferSize);
			}
			if(nFileRealSize >= 8)
			{
				compressSize = m_CompressBufferSize;
				int ret = Zlib_Compress(m_CompressBuffer, &compressSize, m_RealFileBuffer, nFileRealSize);
				if (ret != Z_OK)
				{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
					char buf[MAX_PATH];
					sprintf_s(buf, MAX_PATH, "Compress error [%s].", filePath);
					MessageBox(0, buf, 0, 0);
#endif
					LordLogError("Compress error [%s].", filePath);
				}
				nCompressSize = compressSize;
			}
			else
			{
				memcpy(m_CompressBuffer, m_RealFileBuffer, nFileRealSize);
				nCompressSize = nFileRealSize;
			}
			lpszOut = m_CompressBuffer;
			break;
		default:
			lpszOut = NULL;	

			return false;
		}
		if(nCompressSize<=0)
		{
			m_sErrorString = "Error in function CLizArchive::CompressData, compress file error!\n";
			//getc(stdin);
			lpszOut = NULL;	
			return true;
		}

		fseek(m_pArchiveFile, 0L, SEEK_END);
		m_nCurrFileOffset = ftell(m_pArchiveFile);
		fseek(m_pArchiveFile, 0, SEEK_END);

		fwrite(lpszOut, nCompressSize, 1, m_pArchiveFile);

		fileNode.m_nUseFlag = FILE_IN_USING;
		fileNode.m_nOffset = m_nCurrFileOffset;
		fileNode.m_nFileRealSize = nFileRealSize;
		fileNode.m_nFileCompressSize = nCompressSize;

		lpszOut = NULL;

		return true;
	}

	void* ResourcePack::OutCompressData(const char* filePath, unsigned int& nCompressSize, unsigned int& nRealSize)
	{
		unsigned char	*lpszOut = NULL;
		unsigned int		nFileRealSize = 0;
		FILE				*fp = NULL;
		unsigned int        compressSize = 0;

		fp = fopen(filePath, "rb");
		if (!fp)
		{
			m_sErrorString = "Error in function CLizArchive::CompressData, the file added is error!\n";
			return NULL;
		}
		fseek(fp, 0L, SEEK_END);
		nFileRealSize = ftell(fp);
		if (nFileRealSize > m_RealFileBufferSize)
		{
			m_RealFileBufferSize = nFileRealSize;
			if (m_RealFileBuffer)
			{
				LordFree(m_RealFileBuffer);
			}
			m_RealFileBuffer = (unsigned char*)LordMalloc(m_RealFileBufferSize);
		}

		nRealSize = nFileRealSize;

		fseek(fp, 0L, SEEK_SET);
		fread(m_RealFileBuffer, nFileRealSize, 1, fp);
		fclose(fp);

		lpszOut = (unsigned char*)LordMalloc(nFileRealSize*2);
		if (nFileRealSize >= 8)
		{
			compressSize = nFileRealSize*2;
			int ret = Zlib_Compress(lpszOut, &compressSize, m_RealFileBuffer, nFileRealSize);
			if (ret != Z_OK)
			{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
				char buf[MAX_PATH];
				sprintf_s(buf, MAX_PATH, "Compress error [%s].", filePath);
				MessageBox(0, buf, 0, 0);
#endif
				LordLogError("Compress error [%s].", filePath);
			}
			nCompressSize = compressSize;
		}
		else
		{
			memcpy(lpszOut, m_RealFileBuffer, nFileRealSize);
			nCompressSize = nFileRealSize;
		}

		return lpszOut;
	}

	const tFileNode* ResourcePack::GetFileList(int& nFileListCount)
	{
		LordAssert(m_pArchiveFile);
		LordAssert(m_pFileNode);

		nFileListCount = m_nFilesListNum;
		return m_pFileNode;
	}

	bool ResourcePack::CopyFileList(const tFileNode* pFileNode, int nFileNodeCount)
	{
		LordAssert(m_pArchiveFile);
		LordAssert(m_pFileNode);

		if(m_nFilesListNum < nFileNodeCount)
		{
			m_sErrorString = "Error in function CLizArchive::CopyFileList, file list number is not enough!\n";
			return false;
		}

		unsigned long nFileOffset = 0;
		nFileOffset = sizeof(tFileNode)*(m_nFilesListNum - nFileNodeCount);

		for(int i=0; i<nFileNodeCount; i++)
		{
			memcpy(&m_pFileNode[i], &pFileNode[i], sizeof(tFileNode));
			m_pFileNode[i].m_nOffset += nFileOffset;
		}

		m_bArchiveFileChange = true;
		return true;
	}

	bool ResourcePack::CopyFileCompressData(const char* lpszArchiveFile, unsigned long nOffset)
	{
		unsigned long nFileSize = 0;
		FILE *pfArchiveFile = NULL;

		LordAssert(lpszArchiveFile);
		LordAssert(m_pArchiveFile);
		LordAssert(m_pFileNode);
		
		pfArchiveFile = fopen(lpszArchiveFile, "rb");
		if(!pfArchiveFile)
		{
			m_sErrorString = "Error in function CLizArchive::CopyFileCompressData, open old archive file error!\n";
			return false;
		}

		fseek(pfArchiveFile, 0, SEEK_END);
		nFileSize = ftell(pfArchiveFile);

		fseek(pfArchiveFile, nOffset, SEEK_SET);
		fseek(m_pArchiveFile, m_nFileListOffset+m_nFilesListNum*sizeof(tFileNode), SEEK_SET);
//		for(unsigned long i=0; i<(nFileSize-nOffset); i++)
//		{
//			fread(&p, sizeof(p), 1, pfArchiveFile);
//			fwrite(&p, sizeof(p), 1, m_pArchiveFile);
//		}
        char* pScr = (char*)LordMalloc(nFileSize-nOffset);
        fread(pScr, nFileSize-nOffset, 1, pfArchiveFile);
        fwrite(pScr, nFileSize-nOffset, 1, m_pArchiveFile);
        
        LordSafeFree(pScr);
        
		fclose(pfArchiveFile);
		pfArchiveFile = NULL;
		m_bArchiveFileChange = true;
		return true;
	}

	const char* ResourcePack::GetErrorString()
	{
		return m_sErrorString.c_str();
	}

	bool ResourcePack::IsFileExist(const char* lpszFileName)
	{
		LordAssert(lpszFileName);
		LordAssert(m_pArchiveFile);
		LordAssert(m_pFileNode);

		for(int i=0; i<m_nFilesListNum; i++)
		{
			if(m_pFileNode[i].m_nUseFlag == FILE_IN_USING && strcmp(m_pFileNode[i].m_sFileName, lpszFileName)==0)
				return true;
		}

		return false;
	}

	int ResourcePack::GetFileNodeByName_fast(const char* lpszFileName)
	{
		LordAssert(lpszFileName);
		LordAssert(m_pArchiveFile);
		LordAssert(m_pFileNode);

		String tempStr = lpszFileName;
		StringUtil::LowerCase(tempStr);

		FileNodeMap::iterator it = m_FileNodeMap.find(tempStr);
		if(it != m_FileNodeMap.end())
		{
			return it->second;
		}

		return -1;
	}

	int ResourcePack::GetFileNodeByName(const char* lpszFileName)
	{
		LordAssert(lpszFileName);
		LordAssert(m_pArchiveFile);
		LordAssert(m_pFileNode);

		String tempStr = lpszFileName;
		StringUtil::LowerCase(tempStr);
        
		for(int i=0; i<m_nFilesListNum; i++)
		{
            String strFileName = m_pFileNode[i].m_sFileName;
            StringUtil::LowerCase(strFileName);
			if (m_pFileNode[i].m_nUseFlag == FILE_IN_USING && strcmp(strFileName.c_str(), tempStr.c_str()) == 0)
				return i;
		}

		return -1;
	}

	void ResourcePack::ReleaseExtraMemory()
	{
		if(m_OutBufferSize > MAX_MEMORY_CACHE)
		{
			LordFree(m_OutBuffer);
			m_OutBuffer = NULL;
			m_OutBufferSize = 0;
		}
		if(m_UnCompressBufferSize > MAX_MEMORY_CACHE)
		{
			LordFree(m_UnCompressBuffer);
			m_UnCompressBuffer = NULL;
			m_UnCompressBufferSize = 0;
		}
	}

	bool ResourcePack::UnCompressData (const tFileNode& fileNode, unsigned int& nRealSize)
	{
		if(m_OutBufferSize > MAX_MEMORY_CACHE)
		{
			LordFree(m_OutBuffer);
			m_OutBuffer = NULL;
			m_OutBufferSize = 0;
		}
		if(m_UnCompressBufferSize > MAX_MEMORY_CACHE)
		{
			LordFree(m_UnCompressBuffer);
			m_UnCompressBuffer = NULL;
			m_UnCompressBufferSize = 0;
		}

		nRealSize = fileNode.m_nFileRealSize;
		if(nRealSize > m_OutBufferSize)
		{
			if(m_OutBuffer)
			{
				LordFree(m_OutBuffer);
			}
			m_OutBufferSize = nRealSize;
			m_OutBuffer = (unsigned char*)LordMalloc(nRealSize);
		}

		switch(fileNode.m_nCompressType)
		{
		case CT_NONE:
			{
				fseek(m_pArchiveFile, fileNode.m_nOffset, SEEK_SET);
				fread(m_OutBuffer, nRealSize, 1, m_pArchiveFile);
			}
			break;
		case CT_ZLIB:
			{
				unsigned int compressSize = fileNode.m_nFileCompressSize;
				if(m_UnCompressBufferSize < compressSize)
				{
					if(m_UnCompressBuffer)
					{
						LordFree(m_UnCompressBuffer);
					}
					m_UnCompressBufferSize = compressSize;
					m_UnCompressBuffer = (unsigned char*)LordMalloc(compressSize);
				}
				fseek(m_pArchiveFile, fileNode.m_nOffset, SEEK_SET);
				fread(m_UnCompressBuffer, compressSize, 1, m_pArchiveFile);
				if(nRealSize >= 8)
				{
					unsigned int unzipSize = nRealSize;
					int nRet = Zlib_UnCompress(m_OutBuffer, &unzipSize, m_UnCompressBuffer, compressSize);
					if( Z_OK != nRet )
						return false;
				}
				else
				{
					memcpy(m_OutBuffer, m_UnCompressBuffer, nRealSize);
				}
				if(m_UnCompressBufferSize > MAX_MEMORY_CACHE)
				{
					LordFree(m_UnCompressBuffer);
					m_UnCompressBuffer = NULL;
					m_UnCompressBufferSize = 0;
				}
			}
			break;
		default:
			return false;
		}

		return true;
	}
}
