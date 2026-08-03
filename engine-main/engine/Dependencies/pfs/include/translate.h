#pragma once

namespace PFSX
{

inline TSID MakeTypeIDByName( char a = ' ', char b = ' ', char c = ' ', char d = ' ')
{
	return ((unsigned int)a) | (((unsigned int)b) << 8) | (((unsigned int)c) << 16) | (((unsigned int)d) << 24);
}

inline PFS::FILEPFSXATTRIB MakeFilePfsxAttributes( TSID decoder, CRC32 crc)
{
	return ((PFS::FILEPFSXATTRIB)decoder | ((PFS::FILEPFSXATTRIB)crc << 32));
}

inline TSID GetEncoderTypeFromFilePfsxAttributes( PFS::FILEPFSXATTRIB attr)
{
	return (TSID)(attr);
}

inline TSID GetDecoderTypeFromFilePfsxAttributes( PFS::FILEPFSXATTRIB attr)
{
	return (TSID)(attr);
}

inline CRC32 GetCRC32FromFilePfsxAttributes( PFS::FILEPFSXATTRIB attr)
{
	return (CRC32)(attr >> 32);
}

class CTranslateBase : public PFS::CUncopyableObject
{
	TSID				m_tsid;
	PFS::CBaseFile*		m_src;
	PFS::CBaseFile*		m_src2;
	PFS::CBaseFile*		m_dst;

protected:
	CTranslateBase( TSID tsid)
		: m_tsid( tsid)
		, m_src( NULL)
		, m_src2( NULL)
		, m_dst( NULL)
	{}
	virtual ~CTranslateBase() {}

public:
	PFS::CBaseFile& GetSource() { assert( m_src); return *m_src; }
	PFS::CBaseFile* GetSource2() { return m_src2; }
	PFS::CBaseFile& GetDestination() { assert( m_dst); return *m_dst; }
public:
	TSID GetID() const { return m_tsid; }

	void SetParam( const void* data, size_t size) { /* dummy implementation */ }
	size_t GetParamSize() const { return 0; }
	const void* GetParam() const { return NULL; }

	void SetSource( PFS::CBaseFile* src) { m_src = src; }
	void SetSource2( PFS::CBaseFile* src2) { m_src2 = src2; }
	void SetDestination( PFS::CBaseFile* dst) { m_dst = dst; }
};

class CEncoder : public CTranslateBase
{
public:
	CEncoder( TSID tsid) : CTranslateBase( tsid) {}
	virtual ~CEncoder() {}
public:
	virtual int DoEncode() = 0;
	virtual CEncoder* Clone() const = 0;
	virtual void Delete() = 0;
};

class CDecoder : public CTranslateBase
{
public:
	CDecoder( TSID tsid) : CTranslateBase( tsid) {}
	virtual ~CDecoder() {}
public:
	virtual int DoDecode() = 0;
	virtual CDecoder* Clone() const = 0;
	virtual void Delete() = 0;
};

class CTranslateManager
{
public:
	// 访问 CTranslateManager 对象
	// 说明：初次访问会进行对象的内部初始化，相对比较慢
	static CTranslateManager& GetInstance();

	// 根据tsid 获取对应的Encoder对象
	// 注意，从GetEncoder返回的Encoder对象需要调用Delete()销毁
	CEncoder* GetEncoder( TSID tisd);

	// 根据tsid 获取对应的Decoder对象
	// 注意，从GetDecoder返回的Decoder对象需要调用Delete()销毁
	CDecoder* GetDecoder( TSID tisd);

	// 静态库中的Encoder注册
	void RegisterEncoder( const CEncoder& rEncoder);

	// 静态库中的Decoder注册
	void RegisterDecoder( const CDecoder& rDecoder);

private:
	CTranslateManager();
	~CTranslateManager();

	// 内部实现
	void _Initialize();
	void _UnInitialize();
	void _OnFindPlugin( const wchar_t* wszFileName );
	bool _RegisterPlugin( const void * pModule );
	void _RegisterEncoder( CEncoder* pEncoder);
	void _RegisterDecoder( CDecoder* pDecoder);

private:
	typedef std::map<TSID,CEncoder*> CEncoderMap;
	typedef std::map<TSID,CDecoder*> CDecoderMap;
	typedef std::list<void*> CPluginModulesContainer;

	CEncoderMap m_EncoderMap;
	CDecoderMap m_DecoderMap;
	CPluginModulesContainer m_PluginModules;
};

class CEncodeFile : public PFS::CFile
{
public:
	CEncodeFile() {}
	virtual ~CEncodeFile() {}
public:
	virtual bool Open( const std::wstring& filename, PFS::FILE_MODE fm, PFS::FILE_ACCESS fa);
};

bool EncodeFile( const std::wstring& srcFileName, const std::wstring& dstFileName );

bool EncodeFile( PFS::CBaseFile* psrcFile, PFS::CBaseFile* pdstFile, TSID encoderId );

class CDecodeFile : public PFS::CFile
{
public:
	CDecodeFile() {}
	virtual ~CDecodeFile() {}
public:
	virtual bool Open( const std::wstring& filename, PFS::FILE_MODE fm, PFS::FILE_ACCESS fa);
};

bool DecodeFile( const std::wstring& srcFileName, const std::wstring& dstFileName );
// DecodeFile的强化版,执行DecodeFile之后并对结果文件进行完整性检测
// 该函数为版本验证工具特别提供
bool DecodeFileEn( const std::wstring& srcFileName, const std::wstring& dstFileName );
bool DecodeFileEn( PFS::CBaseFile& inFile, PFS::CBaseFile& outFile, const PFS::CMetaInfo& minfo );
bool DecodeFile( PFS::CBaseFile* psrcFile, PFS::CBaseFile* pdstFile, TSID decoderId );

// make difference of two the files( oldFileName and newFileName) 
bool DiffFile( const std::wstring& oldFileName, const std::wstring& newFileName, const std::wstring& diffFileName );
bool DiffFile( PFS::CBaseFile* oldFile, PFS::CBaseFile* newFile, PFS::CBaseFile* diffFile );

// Patch oldFileName using the file of patchFileName, the result is also oldFileName.
// TSID of patchFileName should be BDF
bool PatchFile( const std::wstring& oldFileName, const std::wstring& patchFileName );

// Patch the file of oldFileName using the file of patchFileName, the result is the file of newFileName
// TSID of patchFileName should be BDF
// newFileName should not be same as oldFileName
bool PatchFile( const std::wstring& oldFileName, const std::wstring& patchFileName, const std::wstring& newFileName );

//////////////////////////////////////////////////////////////////////////
// 使用Translate方式Open文件(同时应用Encoder/Decoder)
class CFileEx : public PFS::CFile
{
public:
	CFileEx() {}
	virtual ~CFileEx() {}
public:
	virtual bool Open( const std::wstring& filename, PFS::FILE_MODE fm, PFS::FILE_ACCESS fa);
};

// 使用Translate方式copy文件(同时应用Encoder/Decoder)
// 文件MetaInfo属性中的m_attributes所代表的 encoder/decoder 都为0 时，目标文件为输入文件的硬拷贝
// 根据encoderId和decoderId对输入文件psrcFile进行转换，将结果写到pdstFile中
// 当encoderId 和 decoderId 都为0 时，目标文件为输入文件的硬拷贝
bool TranslateFile( const std::wstring& srcFileName, const std::wstring& dstFileName );
bool TranslateFile( PFS::CBaseFile* psrcFile, PFS::CBaseFile* pdstFile, TSID encoderId, TSID decoderId );

//////////////////////////////////////////////////////////////////////////
// 校验文件
// 注rFile参数的函数，在执行完Check过程后，文件指针不一定在文件头，
// 如果有需要，调用方Seek到文件头。
//////////////////////////////////////////////////////////////////////////

// 根据MetaInfo提供的FileSize和FileHash值校验文件内容是否完整
bool CheckFile( const std::wstring& strFileName );

bool CheckFile( PFS::CBaseFile& rFile, const PFS::CMetaInfo& minfo );

// 根据MetaInfo提供的FileSize2和FileHash2值校验文件内容是否完整
bool CheckFile2( const std::wstring& strFileName );
bool CheckFile2( PFS::CBaseFile& rFile, const PFS::CMetaInfo& minfo );

// 如果minfo中的tsid=X2O或0，则调用CheckFile校验文件内容，
// 如果FileSize2和FileHash2都为0，即现在的MetaInfo是1.0,我们不做校验直接pass，
// 否则，我们使用CheckFile2进行校验。
bool CheckFileEx( const std::wstring& strFileName );
bool CheckFileEx( PFS::CBaseFile& rFile, const PFS::CMetaInfo& minfo );

} // namespace PFSX {

