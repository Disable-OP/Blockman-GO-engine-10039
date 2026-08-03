///
/// 为server实现网络提供商属性： <server type="" url="" network=""/> 
/// 2011-3-16
///
#pragma once
#include <string>
#include <map>
#include <sstream>
#include "../xmlio/xmlio.h"

namespace PFSX
{
	// 将dword版本号转为字符串 "x.y.z" 形式
	// 31-24位为x ,23-16位为y , 15-0位为z
	inline bool Version2String( DWORD dwVersion, std::wstring& str)
	{
		std::wostringstream wos;
		wos << ( (dwVersion >> 24) & 0x000000ff) << L"." 
			<< ( (dwVersion >> 16) & 0x000000ff) << L"." 
			<< (dwVersion & 0x0000ffff);
		str = wos.str();
		return true;
	}

	// 将字符串 "x.y.z" 形式转为dword版本号
	// 31-24位为x ,23-16位为y , 15-0位为z
	inline bool String2Version( const std::wstring& src, DWORD& dwVersion)
	{
		dwVersion = 0;
		std::wistringstream ss( src);
		std::wstring token;
		int i = 0;
		while (true)
		{
			std::getline( ss, token, L'.');
			if( ss.fail())
				break;

			if( i == 0)
				dwVersion |= ((BYTE)_wtoi( token.c_str())) << 24;
			else if( i == 1)
				dwVersion |= ((BYTE)_wtoi( token.c_str())) << 16;
			else if( i == 2)
				dwVersion |= _wtoi( token.c_str());
			i++;
		}
		if( i != 3)
			return false;
		return true;
	}

	const std::wstring VERSION_META_NAME = L"/.version.meta";
	const std::wstring VERSION2_META_NAME = L"/.version2.meta";
	const wchar_t TYPE[] = L"type";
	const wchar_t VERSION[] = L"version";
	const wchar_t VERSIONSTRING[] = L"versionstring";
	const wchar_t LASTVERSION[] = L"lastversion";
	const wchar_t RELEASEDATE[] = L"release-date";
	const wchar_t VALID[] = L"valid";
	const wchar_t LUV[] = L"luv";		// lowest update version
	const wchar_t URL[] = L"url";
	const wchar_t NETWORK[] = L"network"; // 网络提供商 network provider
	const wchar_t HASH[] = L"hash";
	const wchar_t CHILD[] = L"server";
	const wchar_t UPDATELOG[] = L"updatelog";
	const wchar_t SIZE[] = L"size";
	const wchar_t UPDATESIZE[] = L"updatesize";

	// 特殊值，非枚举类型
	const int VERSION_ALL_TYPES	= (int)(-1); // 所有版本

	//
	// 对应于VersionUpdateInfo中的type
	//
	enum VERSION_TYPE
	{
		VERSION_DEFAULT		=	0,	// 缺省更新(差异更新)
		VERSION_DIFFERENTIAL=	0,	// 差异更新
		VERSION_FULL		=	1,	// 全部更新（零安装方式）
		VERSION_BACKGROUND	=	2,	// 后台更新
		VERSION_COUNT		=	3,	// 版本类型总数
		VERSION_PATCH		=	100,// 补丁包自动更新

		VERSION_SERVER		=	200, //版本文件服务器(用于launcher启动时判断本地版本是否需要更新)
	};
	
	//
	// 特别说明，对于BRIEF的Mount点来说，它无法自选VERSION类型，
	// 因此，使用前需将特殊VERSION类型更改为DEFAULT。
	//

	struct VersionUpdateInfo
	{
		int					Type;
		std::wstring		Url;
		std::wstring		Network; // 网络提供商：网通/电信，台湾，……

		VersionUpdateInfo()
		{
			Type = VERSION_DEFAULT;
		}

		VersionUpdateInfo(int nType, std::wstring strUrl)
		{
			Type = nType;
			Url.swap(strUrl);
		}

		VersionUpdateInfo(int nType, std::wstring strUrl, std::wstring strNetwork)
		{
			Type = nType;
			Url.swap(strUrl);
			Network.swap(strNetwork);
		}
	};

	class CVersionInfo
	{
	public:
		DWORD			m_Version;		// Version: Major.Minor.Build
		DWORD			m_lastVersion;	// 上一版本版本号，只有差异包有这个属性
		DWORD			m_luv;			// 最低可更新版本号属性
		long			m_ReleaseDate;	// __time32_t
		std::wstring	m_Hash;
		std::wstring	m_UpdateLog;	// updatelog
		double			m_size;			// 安装包大小，单位为M，该大小只是估算值
		double			m_sizeUpdate;	// 补丁包大小，单位为M，该大小只是估算值，只有差异包有这个属性
		bool			m_bValid;		// 当前版本是否有效，用于repair工具记录修复失败的标记
										// 因为,即使当前版本与服务器版本一致,当使用repair工具
										// 修复客户端失败后,此版本信息已经无法反映客户端真实状态,
										// 此时,置此标志,patcher程序可根据此标记修复客户端.

		// 缺省构造无效的版本信息
		CVersionInfo()
		{
			m_Version = 1;
			m_lastVersion = 0;
			m_ReleaseDate = 0;
			m_size = 0.0;
			m_sizeUpdate = 0.0;
			m_luv = 0;
			m_bValid = true; // 此字段由repair工具置false
		}

		CVersionInfo(DWORD	_Version, std::wstring	_Hash)
			: m_Version(_Version)
			, m_Hash( _Hash )
			, m_lastVersion( 0 )
			, m_luv( 0 )
			, m_ReleaseDate( 0 )
			, m_size( 0.0)
			, m_sizeUpdate( 0.0 )
			, m_bValid( true )
		{
		}

		bool operator == ( const CVersionInfo& rOther ) const
		{
			return //m_bValid && rOther.m_bValid && // 注意两个版本信息必须都有效方可比较
				m_Version == rOther.m_Version &&
				m_ReleaseDate == rOther.m_ReleaseDate &&
				m_Hash == rOther.m_Hash;
		}

		bool operator != ( const CVersionInfo& rOther )  const
		{
			return !operator ==(rOther);
		}

		//
		// 下面的映射函数只用于VersionMetaFile的处理
		//
		static int ErrorCodeXML2PFS( int xio_ec )
		{
			switch ( xio_ec )
			{
			case XMLIO::EC_SUCCESS:
				return PFS::EC_SUCCESS;
			case XMLIO::EC_CREATE_XML_DOCUMENT:
			case XMLIO::EC_PARSE_XML_CONTEXT:
			case XMLIO::EC_FILE_SIZE_LACK:
			case XMLIO::EC_GET_ROOT_ELEMENT:
			case XMLIO::EC_XML_ENCODE:
			default:
				return PFS::EC_INVALID_VERSION_INFO;
			}
		}

		typedef std::map<int, VersionUpdateInfo> VersionUpdateInfoMap;
		VersionUpdateInfoMap& GetUpdateInfoMap() { return m_updateInfoMap; }
		const VersionUpdateInfoMap& GetUpdateInfoMap() const { return m_updateInfoMap; }

		// Check if valid version info
		bool IsValid() const { return m_bValid; }

		// If not found, returns NULL.
		const VersionUpdateInfo* GetUpdateInfoByType( int type ) const
		{
			const VersionUpdateInfo* pVersion = NULL;
			VersionUpdateInfoMap::const_iterator itr = m_updateInfoMap.find( type );
			if ( itr != m_updateInfoMap.end() )
			{
				pVersion = &itr->second;
			}

			return pVersion;
		}

		bool AddUpdateInfo( const VersionUpdateInfo& rUpdateInfo ) 
		{
			VersionUpdateInfoMap::_Pairib ib = m_updateInfoMap.insert( std::make_pair( rUpdateInfo.Type, rUpdateInfo ) );
			return ( ib.second );
		}

		bool ReadNode( const XMLIO::CINode& root)
		{
			m_Version = root.GetAttributeUnsignedInteger( VERSION ) ;
			m_lastVersion = root.GetAttributeUnsignedInteger( LASTVERSION );
			root.GetAttribute( HASH, m_Hash );
			m_ReleaseDate =  root.GetAttributeInteger( RELEASEDATE);
			m_size = root.GetAttributeFloat( SIZE);
			m_sizeUpdate = root.GetAttributeFloat( UPDATESIZE);
			m_luv = root.GetAttributeUnsignedInteger( LUV, 0 );

			m_bValid = true; // default value
			std::wstring strValue;
			if( root.GetAttribute( VALID, strValue ) )
			{
				// if( strValue == L"true" || strValue == L"1" )
				//	m_bValid = true;
				if( strValue == L"false" || strValue == L"0" )
					m_bValid = false;
			}

			size_t count = root.GetChildrenCount();
			for ( size_t i = 0; i < count; i ++ )
			{
				XMLIO::CINode child;
				root.GetChildAt(i, child);
				if ( child.GetName() == CHILD )
				{
					int type = child.GetAttributeInteger(TYPE);
					std::wstring strUrl;
					child.GetAttribute( URL, strUrl);

					std::wstring strNetwork;
					child.GetAttribute( NETWORK, strNetwork);

					VersionUpdateInfoMap::_Pairib ib = m_updateInfoMap.insert( 
						std::make_pair( type, VersionUpdateInfo(type, strUrl, strNetwork) ) );
					if ( !ib.second )
					{
						// duplicate version?
					}
				}
				else if( child.GetName() == UPDATELOG)
				{
					// 取CDATA节点的文本内容
					m_UpdateLog = child.GetSubText();
				}
				else
				{
					// unknown child
				}
			}

			if ( count == 0 )
			{
				m_bValid = false;
				PFS::CEnv::SetLastError( PFS::EC_INVALID_VERSION_INFO );
			}

			return count > 0;
		}

		bool LoadFrom( PFS::CBaseFile& file)
		{
			XMLIO::CFileReader reader;

			int xec = reader.OpenFile( file);
			if( xec != XMLIO::EC_SUCCESS )
			{
				PFS::CEnv::SetLastError( ErrorCodeXML2PFS( xec ) );
				return false;
			}

			XMLIO::CINode root;
			reader.GetRootNode( root );
			if( !ReadNode( root) )
			{
				return false;
			}

			return true;
		}

		bool WriteNode( XMLIO::CONode& root, int type = VERSION_ALL_TYPES) const
		{
			root.SetAttribute( VERSION, (unsigned int)m_Version );
			std::wstring versionString;
			Version2String( m_Version, versionString);
			root.SetAttribute( VERSIONSTRING, versionString);
			root.SetAttribute( LASTVERSION, (unsigned int)m_lastVersion );
			root.SetAttribute( HASH, m_Hash );
			root.SetAttribute( RELEASEDATE, m_ReleaseDate );
			root.SetAttribute( SIZE, m_size);
			root.SetAttribute( UPDATESIZE, m_sizeUpdate);
			if( m_luv != 0 )
				root.SetAttribute( LUV, (unsigned int)m_luv );

			if( !m_bValid )
			{
				// 只写无效值
				root.SetAttribute( VALID, L"false");
			}

			VersionUpdateInfoMap::const_iterator itr = m_updateInfoMap.begin();
			for( ;itr != m_updateInfoMap.end(); ++ itr )
			{
				const VersionUpdateInfo& rVersionInfo = itr->second;
				if ( type == VERSION_ALL_TYPES )
				{
					XMLIO::CONode child;
					root.CreateSubElement( CHILD, child );
					child.SetAttribute( TYPE, rVersionInfo.Type);
					child.SetAttribute( URL, rVersionInfo.Url);
					if( !rVersionInfo.Network.empty() )
						child.SetAttribute( NETWORK, rVersionInfo.Network);
				}
				else
				{
					 if( rVersionInfo.Type == type )
					 {
						 XMLIO::CONode child;
						 root.CreateSubElement( CHILD, child );
						 child.SetAttribute( TYPE, VERSION_DEFAULT); // Change the type to Default
						 child.SetAttribute( URL, rVersionInfo.Url);
						 if( !rVersionInfo.Network.empty() )
							child.SetAttribute( NETWORK, rVersionInfo.Network);
						 break;
					 }
				}
			}

			if( !m_UpdateLog.empty() )
			{
				// write updatelog
				XMLIO::CONode oNode;
				if( root.CreateSubElement( UPDATELOG, oNode) )
				{
					oNode.InsertCData( m_UpdateLog);
				}
			}

			return true;
		}

		bool WriteTo( PFS::CDataBlock& out, int type = VERSION_ALL_TYPES ) const
		{
			XMLIO::CFileWriter writer;
			XMLIO::CONode root;
			writer.GetRootNode(root);
			
			WriteNode( root, type );
			int xec = writer.SaveData( out);
			if( xec != XMLIO::EC_SUCCESS )
			{
				PFS::CEnv::SetLastError( ErrorCodeXML2PFS( xec ) );
				return false;
			}

			return true;
		}

		bool SaveTo( PFS::CBaseFile& rFile, int type = VERSION_ALL_TYPES ) const
		{
			PFS::CDataBlock out;
			if( !WriteTo( out, type ) )
			{
				return false;
			}

			// UTF16 file prefix
			if( rFile.Write( "\xFF\xFE", 2 ) != 2 )
			{
				return false;
			}

			// Data body
			size_t szTotal = out.GetSize();
			size_t szWritten = 0;
			size_t szBlock;
			while ( szWritten < szTotal )
			{
				szBlock = rFile.Write( (const char*)out.GetData() + szWritten, szTotal-szWritten );
				if( PFS::FILE_SIZE_ERROR == szBlock )
				{
					break;
				}
				szWritten += szBlock;
			}
			
			return szWritten == szTotal;
		}

		void Close() 
		{
			m_Version = 1;
			m_lastVersion = 0;
			m_luv = 0;
			m_ReleaseDate = 0;
			m_size = 0.0;
			m_sizeUpdate = 0.0;
			m_bValid = false;
			m_Hash.clear();
			m_UpdateLog.clear();
			m_updateInfoMap.clear(); 
		}

	private:
		VersionUpdateInfoMap m_updateInfoMap;
	};

	/// <summary>
	/// 	从本地目录获取版本信息
	/// </summary>
	/// <param name="dir">
	/// 	本地目录
	/// </param>
	/// <param name="rLocalVersion">
	/// 	获取到的版本信息
	/// </param>
	/// <return type="bool">
	/// 	是否成功
	/// </return>
	/// <signature>
	/// 	<username>liudongxue</username>
	/// 	<date>2008-12-19</date>
	/// </signature>
	inline bool GetLocalVersion( const std::wstring& dir, CVersionInfo& rLocalVersion)
	{
		std::wstring filename = dir + VERSION_META_NAME;
		if ( dir.empty() )
		{
			PFS::CEnv::SetLastError(PFS::EC_INVALID_DIRNAME);
			assert( !"无效的目录名!" );
			return false;
		}

		PFS::CFile file;
		if( !file.Open( filename, PFS::FM_EXCL, PFS::FA_RDONLY))
			return false;

		if( !rLocalVersion.LoadFrom( file))
			return false;
		
		return true;
	}
	/// <summary>
	/// 	获取远程服务器版本
	/// </summary>
	/// <param name="url">
	/// 	服务器URL
	/// </param>
	/// <param name="rRemoteVersion">
	/// 	远程版本信息
	/// </param>
	/// <return type="bool">
	/// 	是否获取成功
	/// </return>
	/// <signature>
	/// 	<username>liudongxue</username>
	/// 	<date>2008-12-19</date>
	/// </signature>
	inline bool GetRemoteVersion( const std::wstring& url, CVersionInfo& rRemoteVersion)
	{
		std::wstring urlVersion = url + VERSION_META_NAME;
		PFSX::CInetFile netfile;
		if( !netfile.Open( urlVersion ))
			return false;

		// 因为网络文件CInetFile对Seek操作支持不是很好
		// 所以先从网络上读到内存中，然后作为CImageFile装载
		PFS::CDataBlock db;
		size_t filelen = (size_t)netfile.GetSize();
		db.Resize( filelen);
		if( filelen != netfile.Read( db.GetData(), filelen))
		{
			return false;
		}

		PFS::CImageFile imf( db);
		if( !rRemoteVersion.LoadFrom( imf))
		{
			return false;
		}
		return true;
	}

	/// <summary>
	/// 	比较版本信息
	/// </summary>
	/// <param name="rLocalVersion">
	/// 	本地版本
	/// </param>
	/// <param name="rRemoteVersion">
	/// 	远程版本
	/// </param>
	/// <return type="bool">
	/// 	是否相等
	/// </return>
	/// <signature>
	/// 	<username>liudongxue</username>
	/// 	<date>2008-12-19</date>
	/// </signature>
	inline bool CompareVersion( const CVersionInfo& rLocalVersion, const CVersionInfo& rRemoteVersion )
	{
		return ( rLocalVersion == rRemoteVersion );
	}

	/// <summary>
	/// 	从本地文件获取版本信息
	/// </summary>
	/// <param name="path">
	/// 	本地meta文件名
	/// </param>
	/// <param name="rVersionInfo">
	/// 	版本元信息
	/// </param>
	/// <return type="bool">
	/// 	是否成功
	/// </return>
	inline bool ReadVersionMetaFile( const std::wstring& path, CVersionInfo& rVersionInfo)
	{
		if ( path.empty() )
		{
			PFS::CEnv::SetLastError(PFS::EC_INVALID_DIRNAME);
			assert( !"无效的meta文件名!" );
			return false;
		}

		PFS::CFile file;
		if( !file.Open( path, PFS::FM_EXCL, PFS::FA_RDONLY))
			return false;

		if( !rVersionInfo.LoadFrom( file))
			return false;

		return true;
	}

	/// <summary>
	/// 	写版本元信息到文件中
	/// </summary>
	/// <param name="path">
	/// 	目标路径，含名字
	/// </param>
	/// <param name="rVersionInfo">
	/// 	版本元信息
	/// </param>
	/// <param name="type">
	/// 	指定是否筛选版本元信息写到磁盘文件的版本信息类型。
	///		该值若为VERSION_ALL_TYPES，则不筛选。
	///		否则将只写特定的类型，并将该类型更改为DEFAULT。
	/// </param>
	/// <return type="bool">
	/// 	是否写成功
	/// </return>
	/// <signature>
	/// 	<username>liudongxue</username>
	/// 	<date>2008-12-19</date>
	/// </signature>
	inline bool WriteVersionMetaFile(
		const std::wstring& path, 
		const CVersionInfo& rVersionInfo, 
		int type = VERSION_ALL_TYPES)
	{
		PFSX::CNativeFile nativeFile;
		if( !nativeFile.Open( path, PFS::FM_CREAT|PFS::FM_TRUNC, PFS::FA_WRONLY ) )
		{
			return false;
		}

		if( !rVersionInfo.SaveTo( nativeFile, type ) )
		{
			return false;
		}

		return true;
	}

}// namespace PFSX