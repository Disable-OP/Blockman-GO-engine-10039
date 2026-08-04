#pragma once

namespace PFS
{

	typedef unsigned long long	fsize_t; // 64bit的文件大小和文件指针位置
	typedef long long			foff_t;	 // 64bit的文件指针相对偏移量

enum FS_TYPE
{
	FST_NATIVE = 0,
	FST_ZIP,
	FST_BRIEF,
	FST_RUNZIP,
	FST_INETHTTP,  // 待实现，直接挂接http url的device。
};

enum
{
	MT_READONLY		= 0,			// 只读
	MT_WRITABLE		= 0x00000001,	// 可写
	MT_RUNTIME		= 0x00000002,	// 运行时系统，只读，不加载metainfo
	MT_CACHEDATA	= 0x00000004,	// 缓存文件数据（主要用于RUNZIP+RUNTIME组合下的特殊类型）
	MT_ZIPUPDATE	= 0x00000009,	// update的数据为zip过的（主要用于pfs包更新用，与MT_RUNTIME互斥），隐含MT_WRITABLE。
	MT_OPTIONAL		= 0x00000010,	// 该设备可有可无，没有时忽略此设备。(当前只用于MountManager)
	MT_TRUNCATE		= 0x00001001,	// 将底层的文件系统在mount时清空，对于zip包系统，清空zip包文件，对于目录，清空目录下所有文件。隐含MT_WRITABLE。
	MT_PREFERDIR	= 0x00002001,	// 如果底层文件系统不存在，CDevice强制创建Directory系统，而非创建PFS包系统。隐含MT_WRITABLE。
	MT_FSTYPEFLAG	= 0x80000000,   // 最高位为1,显式设定的FS_TYPE类型，如果为0，则自动识别。
	MT_FSTYPEMASK	= 0x7F000000,   // 当MT_FSTYPEFLAG置1时，次高7位为显式设定的FS_TYPE类型；MT_FSTYPEFLAG为0时，不当做FS_TYPE解释。
};
typedef unsigned int MOUNT_TYPE;

inline bool IsMountReadOnly( MOUNT_TYPE mt )
{
	return (mt & MT_WRITABLE) == 0;
}

inline bool IsMountCacheData( MOUNT_TYPE mt )
{
	return (mt & MT_CACHEDATA) != 0;
}

// 是否处理.files.meta(CMetaInfoMap)信息
inline bool IsMountRuntime( MOUNT_TYPE mt )
{
	return (mt & MT_RUNTIME ) != 0;
}

inline bool IsMountZipUpdate( MOUNT_TYPE mt )
{
	return (mt & MT_ZIPUPDATE) == MT_ZIPUPDATE;
}

inline bool IsMountOptional( MOUNT_TYPE mt )
{
	return (mt & MT_OPTIONAL) != 0;
}

inline bool IsMountTruncate( MOUNT_TYPE mt )
{
	return ( mt & MT_TRUNCATE ) == MT_TRUNCATE;
}

inline bool IsMountPreferDir( MOUNT_TYPE mt )
{
	return (mt & MT_PREFERDIR ) == MT_PREFERDIR;
}

// Mount是否指定了FS_TYPE
inline bool IsMountSetFSType( MOUNT_TYPE mt )
{
	return (mt & MT_FSTYPEFLAG ) == MT_FSTYPEFLAG;
}

// 如果IsMountSetFSType(mt)为true, 从Mount类型中获取FS_TYPE
inline FS_TYPE FSTypeFromMountType( MOUNT_TYPE mt )
{
	return (FS_TYPE)((mt & MT_FSTYPEMASK ) >> 24);
}

// 构造带有FSType的MountType
inline MOUNT_TYPE MakeMountTypeWithFSType( MOUNT_TYPE mt, FS_TYPE fst )
{
	return mt | MT_FSTYPEFLAG | (MOUNT_TYPE)fst << 24 ;
}

enum 
{
	FM_EXCL         = 0x00000001,  /*! 文件必须存在 */
	FM_CREAT        = 0x00000002,  /*! 创建文件 */
	FM_TRUNC        = 0x00000004,  /*! 清除文件内容 */
};
typedef unsigned int FILE_MODE;

enum FILE_ACCESS
{
	FA_RDONLY       = 0x00000001,	/*! open for reading only */
	FA_WRONLY       = 0x00000002,	/*! open for writing only */
	FA_RDWR         = 0x00000003,	/*! open for reading and writing */
};

// 文件是否以只读方式打开
inline bool IsFileReadOnly( FILE_ACCESS fa ) 
{
	return (fa & FA_WRONLY) == 0;
}

// 文件是否以可写方式打开
inline bool IsFileWritable( FILE_ACCESS fa )
{
	return (fa & FA_WRONLY) != 0;
}

// 
// enumeration of pfs option groups
//
enum 
{
	PFS_OPTION_GLOBAL = 0,		// global properties that can be identified by property name.
	PFS_OPTION_LOG = 1,			// log properties(such as log level, log file... reserved)
	PFS_OPTION_INTERNET = 2,	// internet properties( such as connect timeout, receive timeout, send timeout. reserved)
	PFS_OPTION_BASE_FILTER = 10,// 可以通过global属性 base-filter 开启或者关闭
								// pfs base path filter( name is the base to be replaced, value is the base to replace)
	PFS_OPTION_BASE_FILESMETAVERSION = 11,// Set the version of a pfs base .files.meta file. 
	// The name must be a base root path, and the value is the version string like 1.0, 1.1, 2.0...
	// 注意：尽管递归mount native设备时，也可以设置此选项，但是，当前PFS不支持为此子系统单独生成不同版本的的filesmetaversion。
};
typedef int PFS_OPTION_GROUP;

enum
{
	EC_SUCCESS		= 0,
	EC_DIR_HAS_ALREADY_MOUNT = -1000,	// 目录已经映射到了一个设备
	EC_INVALID_MOUNT_DEVICE,	// 无效的Mount设备
	EC_INVALID_DEVICE_NAME,		// 无效的Mount设备名
	EC_INVALID_MOUNT_DIR_NAME,	// 无效的Mount目标目录名
	EC_FSTYPE_UNEXISTING,		// 文件系统类型不存在
	EC_MOUNT_DIR_UNFOUND,		// 无法找到该被映射的目录
	EC_MOUNT_FAILED,			// Mount 文件系统失败
	EC_FS_BUSY_NOW,				// 文件系统正在被使用
	EC_FILE_NOT_MOUNT,			// 没有找到相应的映射目录
	EC_FILE_NOT_FOUND,			// 没有找到文件
	EC_FS_READONLY,				// 文件系统目前只读
	EC_FILE_OPEN_MODE,			// 文件打开方式不支持
	EC_NATIVE_DEVICE_LOST,		// 无法找到原始文件设备
	EC_COPY_DST_EXISTING,		// 文件拷贝的目标已经存在
	EC_MAP_UNFOUND,				// 文件名找不到映射
	EC_FILE_BAD_ZIPDATA,		// zip压缩的数据解压缩失败
	EC_FILE_READONLY,			// 当前文件只读
	EC_FILE_URLOPENERROR,		// url打开错误
	EC_INVALID_VERSION_INFO,	// 无效的版本信息
	EC_INVALID_FILENAME,		// 无效的文件名
	EC_INVALID_DIRNAME,			// 无效的目录名
	EC_DIR_NOT_FOUND,			// 目录不存在(无法删除)
	EC_DIR_ALREADY_EXISTING,	// 目录已存在(不能再次创建)
	EC_DIR_NOT_EMPTY,			// (不允许删除)非空目录
	EC_ACCESS_DENIED,			// 访问被拒绝(权限不足)
	EC_SHARING_VIOLATION,		// 访问违例(比如删除一个已打开的文件)
	EC_UNMOUNT_FAILED,			// UnMount失败
	EC_INVALID_PATCH,			// 无效的补丁包
	EC_PATCH_NOT_MATCH,			// 补丁包版本不匹配本地客户端
	EC_FILE_NOTOPEN,			// 文件尚未打开
	EC_FILE_WRITEONLY,			// 文件只支持写操作
	EC_INSUFFICIENT_BUFFER,		// 缓冲区不足以容纳所请求的数据
	EC_FILESIZE_TOO_LARGE,		// 文件大小超过内存所允许的最大值,无法完成此操作.
	EC_INVALID_META_FILE,		// 不合法的MetaFile文件格式
	EC_UNSUPPORT_META_FILE_VERSION,	// 无效的MetaFile版本或MetaFile版本太高

	EC_FILTER_ERROR			= -1100,// 一般过滤器错误
	EC_FILTER_INVALID_ORI_DIR,		// 无效的过滤器原目录
	EC_FILTER_INVALID_NEW_DIR,		// 无效的过滤器新目录
	EC_FILTER_ITEM_NOT_EXISTS,		// 不存在此过滤器配置项目
	
	EC_OPTION_GENERAL_ERROR = -1200,// 一般属性错误
	EC_OPTION_INVALID_GROUP,		// 无效的属性组
	EC_OPTION_INVALID_NAME,			// 无效的属性名
	EC_OPTION_INVALID_VALUE,		// 无效的属性值
	EC_OPTION_OUT_OF_RANGE,			// 属性值超出可允许的范围
	EC_OPTION_READ_ONLY,			// 属性只读（不支持修改此属性）
	EC_OPTION_SYSTEM_ERROR,			// 属性修改/读取时发生系统错误

	EC_DEVICE_INVALID_DEVNAME = -1300, // 无效Device名称
	EC_DEVICE_INVALID_MOUNTTYPE,	// 无效的MOUNT类型
	EC_DEVICE_INVALID_FSTYPE,		// 无效的文件系统
	EC_DEVICE_MISSING_STORAGE,		// 只读MOUNT时，没有存储设备
	EC_DEVICE_NOT_INIT,				// 设备尚未初始化
	EC_DEVICE_ALREADY_MOUNT,		// 设备已经被挂接
	EC_DEVICE_ALREADY_UNMOUNT,		// 设备已经被卸载
	EC_DEVICE_FILE_INVALID_PARAMETER, // 无效的DEVICE FILE参数。
	
	ECX_ENCODER_UNFOUND		= -2000,// 文件编码器没有找到
	ECX_DECODER_UNFOUND,			// 文件解码器没有找到
	ECX_BADFORMAT,					// 错误的文件格式
};

#define MAX_MEMORY_DATA_SIZE  ((size_t)(-1)) // 内存中的数据上限

enum { FILE_SIZE_ERROR = -1, };
enum FILE_SEEK_METHOD
{
	FSM_SET = 0,
	FSM_CUR,
	FSM_END,
};

class CBaseFile : public CUncopyableObject
{
	friend class CEnv;
public:
	CBaseFile() {}
	virtual ~CBaseFile() {}
public:
	virtual size_t Read( void* data, size_t size) = 0;
	virtual size_t Write( const void* data, size_t size) = 0;
	virtual fsize_t GetSize() = 0;
	virtual fsize_t Seek( foff_t pos, FILE_SEEK_METHOD fsm) = 0;
	virtual fsize_t GetPos() = 0;
	virtual bool SetEnd() = 0;	
	virtual CImageData GetImage() = 0;
	//
	// 缓冲区data必须至少能够容纳全部文件数据,否则返回false, CEnv::GetLastError()返回EC_INSUFFICIENT_BUFFER.
	// 尽管该函数的第二个参数类型为fsize_t, 但对于超过size_t(-1)的文件(4G),该函数总是返回false, 
	// 且CEnv::GetLastError()返回EC_FILESIZE_TOO_LARGE.
	//
	virtual bool ReadAll( void* data, fsize_t size ) = 0; 
protected:
	virtual void Delete() { delete this; }
};

PFS::CImageData BaseFileToImageData( PFS::CBaseFile& file); //	读取文件数据到CDataBlock对象，再转换成CImageData

typedef unsigned long long	FILE_DATA_HASH;	// 文件内容Hash值
typedef unsigned long long	FILE_NAME_HASH;	// 文件路径Hash值
typedef unsigned long long	FILEPFSXATTRIB;
typedef unsigned int		TSID;
typedef unsigned int		CRC32;

class CMetaInfo
{
	friend class CMarshalMetaInfo;
	friend class CMarshalMetaInfo_1_1;

	FILE_DATA_HASH			m_dataHash;		// 文件内容Hash值
	FILE_NAME_HASH			m_pathHash;		// 未序列化到文件
	std::wstring			m_uri;			// 未序列化到文件
	fsize_t					m_sizeFile;		// 以变长数据序列化到文件
	int						m_nMethod;		// zip level(以unsigned char方式序列化到文件)
	union 
	{
		FILEPFSXATTRIB		m_attributes;	// for pfsx use
		struct 
		{
			TSID			m_tsid;
			CRC32			m_crc32;
		};
	};

	// 
	// 1.1 版本加入
	// 
	// 对于 tsid为 X2O 和 0 的文件来说，下面的2个字段值等同于m_dataHash和m_sizeFile
	// 这种情况下，不序列化到文件中,以节省文件空间.
	//
	fsize_t					m_sizeFile2;	// 变换格式后的文件大小，以变长数据序列化到文件
	FILE_DATA_HASH			m_dataHash2;	// 变换格式后的文件内容Hash值(压缩)

public:
	CMetaInfo() 
		: m_dataHash(0)
		, m_pathHash(0)
		, m_sizeFile(0)
		, m_nMethod(0)
		, m_attributes(0)
		, m_sizeFile2(0)
		, m_dataHash2(0)
	{}

	CMetaInfo( const CMetaInfo& src) 
		: m_dataHash( src.m_dataHash)
		, m_pathHash( src.m_pathHash)
		, m_uri( src.m_uri)
		, m_sizeFile( src.m_sizeFile)
		, m_nMethod( src.m_nMethod)
		, m_attributes( src.m_attributes)
		, m_sizeFile2( src.m_sizeFile2)
		, m_dataHash2( src.m_dataHash2)
	{}

	~CMetaInfo() 
	{}
public:
	CMetaInfo& operator=( const CMetaInfo& src)
	{
		if( this == &src) 
			return *this;
		m_dataHash = src.m_dataHash;
		m_dataHash2 = src.m_dataHash2;
		m_pathHash = src.m_pathHash;
		m_uri = src.m_uri;
		m_sizeFile = src.m_sizeFile;
		m_sizeFile2 = src.m_sizeFile2;
		m_nMethod = src.m_nMethod;
		m_attributes = src.m_attributes;
		return *this;
	}
	bool operator != ( const CMetaInfo& right)
	{
		return (m_dataHash != right.m_dataHash) 
			|| (m_pathHash != right.m_pathHash) 
			|| (m_sizeFile != right.m_sizeFile) 
			|| (m_attributes != right.m_attributes)
			|| (m_nMethod != right.m_nMethod) 
			|| (m_uri != right.m_uri)
			|| (m_dataHash2 != right.m_dataHash2) 
			|| (m_sizeFile2 != right.m_sizeFile2) 
			;
	}

	const FILE_DATA_HASH& GetFileHash() const { return m_dataHash; }
	void SetFileHash( const FILE_DATA_HASH& dataHash) { m_dataHash = dataHash; }

	const FILE_DATA_HASH& GetFileHash2() const { return m_dataHash2; }
	void SetFileHash2( const FILE_DATA_HASH& dataHash2) { m_dataHash2 = dataHash2; }

	const FILE_NAME_HASH& GetFilePathHash() const { return m_pathHash; }
	void SetFilePathHash( const FILE_NAME_HASH& pathHash) { m_pathHash = pathHash; }

	const std::wstring& GetFileDownloadURI() const { return m_uri; }
	void SetFileDownloadURI( const std::wstring& strURI) { m_uri = strURI; }

	FILEPFSXATTRIB GetFilePfsxAttributes() const { return m_attributes; }
	void SetFilePfsxAttributes( FILEPFSXATTRIB attr) { m_attributes = attr; }

	TSID GetTSID() const { return m_tsid; }
	void SetTSID( TSID tsid) { m_tsid = tsid; }

	CRC32 GetCRC32() const { return m_crc32; }
	void SetCRC32( CRC32 _crc32) { m_crc32 = _crc32; }

	fsize_t GetFileSize() const { return m_sizeFile; }
	void SetFileSize( fsize_t size) { m_sizeFile = size; }

	fsize_t GetFileSize2() const { return m_sizeFile2; }
	void SetFileSize2( fsize_t size2) { m_sizeFile2 = size2; }

	int GetMethod() const { return m_nMethod; }
	void SetMethod( int nMethod) { m_nMethod = nMethod; }

	// 判断是否是V10版本的MetaInfo
	bool IsV10() const { return m_sizeFile2 == 0 && m_dataHash2 == 0; }
};

typedef std::map<std::wstring,CMetaInfo> CMetaInfoMap;

class CFileHashMethod  : public CUncopyableObject
{
public:
	CFileHashMethod() {}
	virtual ~CFileHashMethod() {}
public:
	virtual CFileHashMethod* Clone() const = 0;
	virtual void Delete() { delete this; }
public:
	virtual bool MakeFileHash( CBaseFile& file, FILE_DATA_HASH& digest) const = 0;
};

class CEnv
{
	CEnv() {}
	~CEnv() {}
public:
	static int GetLastError();

	// 说明: 尽管这里提供了SetLastError函数，但它应该仅用于PFS/PFSX名称空间
	// 及其子空间的实现代码中，而不要用于使用PFS的应用程序中
	static void SetLastError(int nErrorCode);

	//
	// PFS 系统属性接口
	//
	// SetOption: 设置PFS系统属性
	// GetOption: 读取PFS系统属性
	// 参数:
	//	group - 属性组ID，参见PFS_OPTION_GROUP定义
	//	name - 指定组下的属性标志
	//  value - 指定组下对应的属性的属性值
	//
	// 返回值:
	//	对于SetOption,设置成功时返回true，对于只读属性，执行SetOption操作，总是会返回false，PFS LastError会设置EC_OPTION_READ_ONLY.
	//  对于GetOption,读取成功时返回true. 对于不存在的属性，总是返回false，PFS LastError会设置EC_OPTION_INVALID_GROUP或EC_OPTION_INVALID_NAME.
	static bool SetOption( PFS_OPTION_GROUP group, const std::wstring& name, const std::wstring& value );
	static bool GetOption( PFS_OPTION_GROUP group, const std::wstring& name, std::wstring& value );

	// 开启或关闭BaseFilter。
	// 为了不影响效率，缺省情况下不开启BaseFilter，除非显式开启.
	// 也可以通过global属性 base-filter 开启或者关闭
	// 开启或关闭成功返回true，开启或关闭失败返回false。
	static bool EnableBaseFilter(bool bEnable);

	// 获取BaseFilter开启与否的状态
	// BaseFilter开启返回true，没有开启返回false。
	static bool IsBaseFilterEnabled();

	// 设置/获取base目录的优先替换目录(过滤器方式)
	// 这2个接口也可以通过SetOption/GetOption(PFS_OPTION_BASE_FILTER,baseDirOriginal,baseDirNew)接口进行操作。
	// 但是，都必须通过EnableBaseFilter(true)开启或通过EnableBaseFilter(false)关闭。
	// filter不存在时，GetBaseFilter()返回false，GetLastError()返回 EC_FILTER_ITEM_NOT_EXISTS。
	static bool SetBaseFilter( const std::wstring& baseDirOriginal, const std::wstring& baseDirNew );
	static bool GetBaseFilter( const std::wstring& baseDirOriginal, std::wstring& baseDirNew );

	//
	// 挂载PFS子系统
	//
	static bool Mount( const std::wstring& dir, const std::wstring& device, FS_TYPE fst, MOUNT_TYPE mt = MT_READONLY);

	// 提交所有文件系统的修改操作，但不unmount设备.
	// 针对可写的device，刷新Mount设备：
	//	1. 对于native fs来说，刷新metafile
	//	2. 对于zip fs 来说，整理设备并且删除临时文件，刷新metafile
	// 对于只读的文件系统，该操作无意义，但是返回true。
	static bool Commit( const std::wstring& dir );

	// 卸载PFS子系统
	// commit = true，在Unmount过程中，执行Commit操作，提交文件系统的相关修改.
	// commit = false, 如果是 FST_ZIP 类型将不进行整理操作，即Rollback（回滚所有修改）.对其他FST类型无区别.
	static bool Unmount( const std::wstring& dir, bool commit = true);

	// 刷新目录 dir 的 files.meta 文件
	static bool RefreshMetaFile( const std::wstring& dir);
	// 得到目录 dir 的 files.meta 文件的hash值
	static bool GetMetaFileHash( const std::wstring& dir, FILE_DATA_HASH& res);

	static bool GetMetaInfos( const std::wstring& dir, CMetaInfoMap& mim);

	// 说明: 尽管这里提供了SetMetaInfos函数，但它应该仅用于特殊应用场合以解决UI处理相关问题。
	// 另外，为了提高效率，这里的mim参数为非const引用，内部会以swap方式替换mim的内容。
	// 因此，此函数调用成功之后，mim的值将会发生变化。
	// 参见BriefFS的RUNTIME模式和PFS配套更新系统的实现方案。
	static bool SetMetaInfos( const std::wstring& dir, CMetaInfoMap& mim);

	static bool GetMetaInfosEx( const std::wstring& dir, CMetaInfoMap& mim);
	static bool CheckMetaInfo( const std::wstring& dir);
	static bool GetFileMetaInfo( const std::wstring& filename, CMetaInfo& info);
	static bool SetFileMetaInfo( const std::wstring& filename, const CMetaInfo& info);
	static bool AddFileMetaInfo( const std::wstring& filename, const CMetaInfo& info);
	static bool IsFileExisting( const std::wstring& filename);

	static void SetFileHashMethod( const CFileHashMethod& fhm);
	static const CFileHashMethod& GetFileHashMethod();
	static void SetDefaultFileHashMethod();

	static CBaseFile* OpenFile( const std::wstring& filename, FILE_MODE fm, FILE_ACCESS fa);
	static bool CloseFile( CBaseFile* file);
	static bool RemoveFile( const std::wstring& filename);
	static bool FileCopy( const std::wstring& src, const std::wstring& dst, bool failifexsting = true);

	static bool NativeFileNameToPFSFileName( const std::wstring& in, std::wstring& out); // if failed,GetLastError = EC_MAP_UNFOUND
	static bool PFSFileNameToNativeFileName( const std::wstring& in, std::wstring& out); // if failed,GetLastError = EC_MAP_UNFOUND

	static void SetOutLogInstance( COutLog* log);
	static COutLog* GetOutLogInstance();

	// 生成指定字符串的hash
	static FILE_DATA_HASH MakeStringHash( const std::wstring& str);
};

} // namespace PFS{

namespace PFSX {
	using PFS::foff_t;
	using PFS::fsize_t;
	using PFS::TSID;
	using PFS::CRC32;
} // namespace PFSX {
