/********************************************************************
filename: 	ResourcePack.h
file path:	dev\engine\Src\Core\Resource\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ZIP_ARCHIVE__
#define __LORD_ZIP_ARCHIVE__

#include "Resource/Threading.h"

#define MAX_FILE_NODE_NUM 20000
#define MIN_FILE_NODE_NUM 50			

#define MAX_MEMORY_CACHE 1024*1024

namespace LORD
{

	enum FILE_USING_TYPE
	{
		FILE_NOT_USING = 0,
		FILE_IN_USING,
		FILE_DELETE
	};

	enum FILE_COMPRESS_TYPE
	{
		CT_NONE = 0,
		CT_ZLIB
	};


	struct tFileNode
	{
		char			m_sFileName[64];		
		ui32			m_nOffset;				
		ui32			m_nFileRealSize;		
		ui32			m_nFileCompressSize;	
		ui8				m_nCompressType;		
		ui8				m_nUseFlag;				

		tFileNode()
		{
			memset(m_sFileName, 0, sizeof(m_sFileName));
			m_nUseFlag = FILE_NOT_USING;
			m_nOffset = 0;
			m_nFileRealSize = 0;
			m_nFileCompressSize = 0;
			m_nCompressType = 0;
		}
	};

	class LORD_CORE_API ResourcePack
	{
	public:
		typedef map<String, int>::type FileNodeMap;
	public:
		ResourcePack();
		~ResourcePack();

	public:
		bool OpenFile(const char* lpszArchiveFile);

		bool CloseFile();
		
		const char* GetVersion();

		const unsigned char* ReadFile(const char* lpszFileName, unsigned int& nOutLength) ;

		void ReleaseExtraMemory();

		bool CopyFileList(const tFileNode* pFileNode, int nFileNodeCount);

		bool CopyFileCompressData(const char* lpszArchiveFile, unsigned long nOffset);

		const tFileNode* GetFileList(int& nFileListCount);

		const char* GetErrorString();

		bool IsFileExist(const char* lpszFileName);

		bool IsFileExist_fast(const char* lpszfileName);

		// for pack tools
		bool CreateFile(const char *lpszArchiveFile, int nFileNodeNum = 512);
		bool AddFile(const char *lpszFilePath, const char *lpszFileName, int nCompressType);
		bool AddFile(const char *lpszFileName, const tFileNode* pFileNode, const char *pData, int nSize);
		bool AddZlibFile(const char *lpszFileName);
		bool DelFile(const char *lpszFileName);
		bool SetVersion(const char* szVersion);
		bool SetArchivePath( const char* apszArchivePath );
		bool GetFileCompressData( const char* aszFileName, char* aszOutPut );
	private:
		int GetEmptyFileNode();
		bool RebuildPacket();
	public:
		bool CompressData(tFileNode& fileNode, const char* filePath, unsigned int& nCompressSize);
		void* OutCompressData(const char* filePath, unsigned int& nCompressSize, unsigned int& nRealSize);
		int	Zlib_Compress(unsigned char* dest,unsigned int* destLen,const unsigned char* source,unsigned int sourceLen);

		bool UnCompressData(const tFileNode& fileNode, unsigned int& nRealSize);

		int Zlib_UnCompress(unsigned char* dest, unsigned int* destLen, const unsigned char* source, unsigned int sourceLen);

	private:
		int GetFileNodeByName(const char* lpszFileName);

		int GetFileNodeByName_fast(const char* lpszFileName);

		void ResetMember();

		void FreeMemory();

	public:
		String				m_sErrorString;		

	private:
		unsigned char*		m_RealFileBuffer;			
		ui32				m_RealFileBufferSize;		
		unsigned char*		m_CompressBuffer;				// 文件缓存(压缩)
		ui32				m_CompressBufferSize;			// 文件缓存大小

		unsigned char*		m_UnCompressBuffer;				
		ui32				m_UnCompressBufferSize;		
		unsigned char*		m_OutBuffer;					
		ui32				m_OutBufferSize;

	private:
		FILE*				m_pArchiveFile;		
		ui32				m_nFileListOffset;				// m_pFileNode数据偏移量	
		char				m_sFileVersion[64];				// 打包文件版本号
		tFileNode*			m_pFileNode;					// 文件结点数据
		i32					m_nFilesListNum;				// 文件数量
		FileNodeMap			m_FileNodeMap;					// 文件目录(快速文件结点查询)
		ui32				m_nCurrFileOffset;		
		char				m_sArchiveFileName[256];		// 文件存储路径
		bool				m_bArchiveFileChange;
		String				m_sArchiveFilePath;				// 当前存档文件路径


		EE_MUTEX(m_UnpressLock);
	};
}
#endif