#pragma once
#include <string>
#include <vector>
#include <map>
#include "../xmlio/xmlio.h"

namespace PFSX
{
	const std::wstring PATCH_META_NAME = L"/.patch.meta";

	struct PatchInfo
	{
		std::wstring		filename;
		fsize_t				size;
		PFS::FILE_DATA_HASH	hash;
		std::wstring		md5; // 128bit, 32 hexadecimal value, empty for old version. 
								 // md5sum, a linux command, compitable.
								 // added for p2sp download mode.

		DWORD				versionFrom;
		DWORD				versionTo;

		bool operator == ( const PatchInfo& other)
		{
			return (versionFrom == other.versionFrom) && (versionTo == other.versionTo);
		}
	};
	
	typedef std::vector< PatchInfo >	CPatchInfoVector;
	typedef std::map<int, PatchInfo>	CPatchInfoMap;

	struct UpdateInfo
	{
		DWORD				versionFrom;
		DWORD				versionTo;
		std::vector<int>	use;

		bool operator < ( const UpdateInfo& right)
		{
			// 先按 versionTo 从大到小排序
			// 如果 versionTo 相等，按 versionFrom 从大到小排序
			if( versionTo != right.versionTo )
			{
				return ( versionTo > right.versionTo);
			}
			return versionFrom > right.versionFrom;
		}
	};

	typedef std::vector<UpdateInfo>					CUpdateInfoVector;

	typedef std::map<DWORD, CUpdateInfoVector>		CUpdateInfoMap;

	class CPatchMetaFile
	{
		DWORD			m_version;
		CPatchInfoMap	m_patches; //通用更新包
		CUpdateInfoMap	m_updates; //通用更新项
		CPatchInfoMap	m_patches2;//专用更新包
		CUpdateInfoMap	m_updates2;//专用更新项

		bool LoadPatches( const XMLIO::CINode& node, bool bSpecial = false);
		bool LoadUpdates( const XMLIO::CINode& node, bool bSpecial = false);

		bool WritePatches( XMLIO::CONode& node, bool bSpecial = false) const;
		bool WriteUpdates( XMLIO::CONode& node, bool bSpecial = false) const;

		bool FillPatches( CPatchInfoMap& pim, const std::vector<int>& use);
		bool FillPatches( CPatchInfoVector& rPatchInfoVector, const std::vector<int>& use, DWORD versionFrom);
		//目前倚天项目暂时使用EX版本
		bool FillPatchesEX( CPatchInfoVector& rPatchInfoVector, const std::vector<int>& use, DWORD versionFrom);
		
		bool FillPatches2( CPatchInfoMap& pim, const std::vector<int>& use);
		bool FillPatches2( CPatchInfoVector& rPatchInfoVector, const std::vector<int>& use, DWORD versionFrom);

	public:
		CPatchMetaFile() : m_version( 1) { }

		bool LoadFrom( PFS::CBaseFile& file);

		bool Write( PFS::CBaseFile& file) const;

		// 根据版本得到通用patch信息
		bool GetPatchInfoFromVersion( PatchInfo& pi, DWORD versionFrom, DWORD versionTo);
		// 根据版本得到通用patch列表
		bool GetPatchInfosFromVersion( CPatchInfoVector& piVector, DWORD versionFrom, DWORD versionTo);
		//目前倚天项目暂时使用EX版本
		bool GetPatchInfosFromVersionEX( CPatchInfoVector& piVector, DWORD versionFrom, DWORD versionTo);

		// 根据版本得到专用patch信息
		bool GetPatchInfoFromVersion2( PatchInfo& pi, DWORD versionFrom, DWORD versionTo);
		// 根据版本得到专用patch列表
		bool GetPatchInfosFromVersion2( CPatchInfoVector& piVector, DWORD versionFrom, DWORD versionTo);

		//根据大小，二选一
		//根据包的大小确定一个最小的包
		bool GetMinPatchInfoFromVersion( PatchInfo& pi, DWORD versionFrom, DWORD versionTo);
		//根据包的大小确定一个最小的包序列
		bool GetMinPatchInfosFromVersion( CPatchInfoVector& piVector, DWORD versionFrom, DWORD versionTo);

		void Close();
		
		CPatchInfoMap& GetPatchInfoMap() { return m_patches; }

		CUpdateInfoMap& GetUpdateInfoMap() { return m_updates; }

		CPatchInfoMap& GetPatchInfoMap2() { return m_patches2; }

		CUpdateInfoMap& GetUpdateInfoMap2() { return m_updates2; }

		// 给编辑器使用
		// 根据 versionFrom 和 versionTo 列出所有能使用的通用补丁包列表
		bool ListAllPatchsFromVersion( std::vector<std::vector<int> >& uses, DWORD versionFrom, DWORD versionTo);
		// 编辑器专用
		// 根据 versionFrom 和 versionTo 列出所有能使用的专用补丁包列表
		bool ListAllPatchsFromVersion2( std::vector<std::vector<int> >& uses, DWORD versionFrom, DWORD versionTo);

	public:
		static bool DoRead( const std::wstring& fileName, CPatchMetaFile& pmf)
		{
			PFS::CFile file;
			if( !file.Open( fileName, PFS::FM_EXCL, PFS::FA_RDONLY ) )
			{
				return false;
			}

			return pmf.LoadFrom( file );
		}

		static bool DoWrite( const std::wstring& fileName, const CPatchMetaFile& pmf)
		{
			PFS::CFile file;
			if( !file.Open( fileName, PFS::FM_CREAT|PFS::FM_TRUNC, PFS::FA_WRONLY ) )
			{
				return false;
			}

			return pmf.Write( file);
		}
	};

	bool GetRemotePatchMetaFile( const std::wstring& url, CPatchMetaFile& pmf);

	// 从sSrc中得到以cDelimiter分界的int数组
	bool GetIntVectorFromStr( std::vector<int>& iv, const std::wstring& sSrc, const wchar_t cDelimiter=L',');

	// 从int数组中得到以cDelimiter分界的字符串sSrc
	bool GetStrFromIntVector( const std::vector<int>& iv, std::wstring& sDes, const wchar_t cDelimiter=L',');

};	// namespace PFSX