///=============================================================================
/// Product   : PFS
/// Project   : Miscellaneous Files
/// Module    : mountmanager.h
/// Copyright : (c) 2009 WanMei, Inc. All rights reserved.
/// Creator   : xuhui, liudongxue
///=============================================================================
/// Description :
/// 	GameClient.exe使用的Mount系统管理器(一般搭配mount.xml使用)
///=============================================================================
/// Revision History :
/// 	V1.0 - 2009-2-18
/// 	V1.1 - 2010-7-13 增加option支持;增加mount filter支持。
///=============================================================================
#pragma once

#include <shlwapi.h>
#pragma comment( lib, "shlwapi.lib")

namespace PFSX {

class CMountManager
{
private:
	std::vector<std::wstring> m_mountPoints;
	CMountManager() {}
	~CMountManager()
	{
		// 如果调用方没有显式地调用UnmoutPFS，析构时自动调用
		Unmount();
	}

private:
	int MountDevicesFromXML( const std::wstring& filename)
	{
		XMLIO::CFileReader fr;
		int nError = 0;
		if( filename[0] == L'/' )
		{
			nError = fr.OpenFile( filename);
		}
		else
		{
			nError = fr.OpenNativeFile( filename);
		}

		if( PFS::EC_SUCCESS != nError)
			return nError;
		XMLIO::CINode rootnode;
		fr.GetRootNode(rootnode);
		int nResult = PFS::EC_SUCCESS;
		XMLIO::CNodeList children;
		rootnode.GetChildren(children);
		for( XMLIO::CNodeList::iterator it = children.begin(); it != children.end(); ++ it)
		{
			XMLIO::CINode& node = *it;
			nError = ParseOneXMLElement( node);
			if( PFS::EC_SUCCESS != nError)
				nResult = nError;
		}
		return nResult;
	}

	int ReplacePathVariables( std::wstring& path )
	{
		static LPCWSTR rgPathVariables[] = 
		{
			L"%module-dir%", // 程序本身所在的目录
			L"%current-dir%", // 程序当前目录
		};

		WCHAR buffer[MAX_PATH+1] = {0};
		int occurrence = 0;
		size_t pos = std::wstring::npos;
		for( int i = 0; i < _countof( rgPathVariables); i ++ )
		{
			LPCWSTR szModuleDirVar = rgPathVariables[i];
			if( (pos = path.find(szModuleDirVar)) != std::wstring::npos )
			{
				if( wcscmp( szModuleDirVar, L"%module-dir%" ) == 0 )
				{
					::GetModuleFileNameW( NULL, buffer, MAX_PATH);
					::PathRemoveFileSpecW( buffer );
					path.replace( pos, wcslen(szModuleDirVar), buffer );
					occurrence ++;
				}
				else if( wcscmp( szModuleDirVar, L"%current-dir%" ) == 0 )
				{
					::GetCurrentDirectoryW( MAX_PATH, buffer );
					::PathRemoveBackslashW( buffer );
					path.replace( pos, wcslen(szModuleDirVar), buffer );
					occurrence ++;
				}
			}
		}

		// 只有当替换一个变量时方执行路径规范化处理
		if( occurrence == 1 )
		{
			WCHAR buffer[MAX_PATH+1] ={0};
			::PathCanonicalizeW( buffer, path.c_str() );
			path = buffer;
		}

		return occurrence;
	}

	int ParseOneXMLElement( XMLIO::CINode& node)
	{
		if ( node.GetType() == XMLIO::NT_ELEMENT ) // Only take care of the element node
		{
			std::wstring name = node.GetName();

			if( 0 == _wcsicmp( name.c_str(), L"option") )
			{
				std::wstring name, value;
				int group = node.GetAttributeInteger( L"group", 0 );
				node.GetAttribute( L"name", name);
				node.GetAttribute( L"value", value);
				int code = ParseOptionElement(group, name, value);
				if( code != 0 )
				{
					std::wstring ignorable_string;
					node.GetAttribute( L"ignorable", ignorable_string);
					bool ignorable = ( 0 == _wcsicmp( ignorable_string.c_str(), L"true") ||
						0 == _wcsicmp( ignorable_string.c_str(), L"1")  );
					if( ignorable )
					{
						std::wstring group1;
						node.GetAttribute( L"group", group1 );
						std::wstring errorInfo = L"WARN: invalid option: group=\"" + group1 + L"\" name=\"" + name + L"\" value=\"" + value + L"\".\n";
						// OutputDebugStringW( errorInfo.c_str() );
						PFS::CEnv::GetOutLogInstance()->Print( errorInfo.c_str() );

						code = 0;
					}
				}
				return code;
			}
			else if( 0 == _wcsicmp( name.c_str(), L"mount"))
			{
				std::wstring device, dir, filter;
				node.GetAttribute( L"device", device);
				ReplacePathVariables( device );
				node.GetAttribute( L"dir", dir);

				node.GetAttribute( L"filter", filter);				
				return ParseMountElement( 
					dir,
					device, 
					filter,
					node.GetAttributeInteger(L"mt", 0) );
			}
			else if( 0 == _wcsicmp( name.c_str(), L"path"))
			{
				std::wstring strPath;
				node.GetAttribute( L"name", strPath );
				return ParsePathElement( strPath );
			}

			// ignore it.
			return 0;
		}
		else
		{
			//if( name[0] == L'#' ) // name is always a non-empty string!
			//{
			//	// such as:
			//	// #comment
			//	// #cdata-section
			//	// #text
			//	// #document
			//	// #document-fragment
			//	return 0;
			//}
			return 0;
		}
	}
	
	int ParseOptionElement( int group, const std::wstring& name, const std::wstring& value )
	{
		bool bOK = PFS::CEnv::SetOption( group, name, value );
		if( bOK )
			return PFS::EC_SUCCESS;
		else
			return PFS::CEnv::GetLastError();
	}

	int ParseMountElement( const std::wstring& dir, const std::wstring& device, const std::wstring& filter, PFS::MOUNT_TYPE mt)
	{
		std::wstring strPfsName = device + L".pfs";
		if( (device[0] == L'/' && PFS::CEnv::IsFileExisting( strPfsName)) ||
			::PathFileExistsW(strPfsName.c_str()) )
		{
			PFS::FS_TYPE zipfs = PFS::IsMountReadOnly( mt ) ? PFS::FST_RUNZIP : PFS::FST_ZIP;
			if( PFS::CEnv::Mount( dir, strPfsName, zipfs, mt))
			{
				m_mountPoints.push_back(dir);
				if( !filter.empty() && !PFS::CEnv::SetBaseFilter( dir, filter ) )
				{
					std::wstring errorInfo = L"WARN: set base filter for " + dir + L" as " + filter + L" failed!\n";
					PFS::CEnv::GetOutLogInstance()->Print( errorInfo.c_str() );
				}
				return PFS::EC_SUCCESS;
			}
		}
		else if( PFS::CEnv::Mount( dir, device, PFS::FST_NATIVE, mt) )
		{
			m_mountPoints.push_back(dir); 

			if( !filter.empty() && !PFS::CEnv::SetBaseFilter( dir, filter ) )
			{
				std::wstring errorInfo = L"WARN: set base filter for " + dir + L" as " + filter + L" failed!\n";
				PFS::CEnv::GetOutLogInstance()->Print( errorInfo.c_str() );
			}
			return PFS::EC_SUCCESS;
		}

		if( PFS::IsMountOptional ( mt ) )
		{
			return PFS::EC_SUCCESS;
		}

		std::wstring errorInfo = L"ERROR: Mount " + device + L" to " + dir + L" failed!\n";
		PFS::CEnv::GetOutLogInstance()->Print( errorInfo.c_str() );

		return PFS::CEnv::GetLastError();
	}

	int ParsePathElement( const std::wstring& path)
	{
		return PFS::EC_SUCCESS;
	}

	bool Unmount()
	{
		if ( m_mountPoints.empty() )
		{
			return true;
		}

		// 按相反顺序unmount文件系统
		bool bRet = true;
		std::vector<std::wstring> failedPfs;
		std::vector<std::wstring>::const_reverse_iterator itr = m_mountPoints.rbegin();
		for( ; itr != m_mountPoints.rend(); ++ itr )
		{
			bRet = PFS::CEnv::Unmount( *itr );
			if ( !bRet )
			{
				failedPfs.push_back( *itr );
			}
		}

		failedPfs.swap( m_mountPoints );
		return m_mountPoints.empty();
	}

	static CMountManager& Instance()
	{
		static CMountManager s_mm;
		return s_mm;
	}

public:
	/// <summary>
	/// 	从filename所描述的xml文件中挂载所有mount点
	/// </summary>
	/// <param name="filename">
	///		描述Mount信息的xml文件(mount.xml)
	///		如果该路径名第一个字符为 '/', 将以pfs方式打开该文件，
	/// 	否则将认为该文件为native filepath，而不是pfs filepath
	/// </param>
	/// <return type="int">
	/// 	PFS 错误码. 0 - success. ~0, failed.
	/// </return>
	static int MountFromXML( const std::wstring& filename )
	{
		CMountManager& mm = Instance();
		return mm.MountDevicesFromXML( filename);
	}

	/// <summary>
	/// 	卸载所有Mount点
	/// </summary>
	static void UnmountPFS()
	{
		CMountManager& mm = Instance();
		mm.Unmount();
	}
};

} // namespace PFSX {