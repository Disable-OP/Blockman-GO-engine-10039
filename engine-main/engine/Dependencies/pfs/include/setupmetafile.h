///=============================================================================
/// Product   : PFS
/// Project   : pfslib
/// Module    : setupmetafile.h
/// Copyright : (c) 2009 WanMei, Inc. All rights reserved.
/// Creator   : liudongxue
///=============================================================================
/// Description :
/// 	CSetupMetaFile - .setup.meta/.master.meta文件的操作处理类
///		.setup.meta/.master.meta 文件为pfs辅助工具和更新程序所依赖的文件。
///	它是客户端安装包的资源文件包清单。
///	出现在此清单中的项目cfg表示cfg目录在打包后成为cfg.pfs资源包。
/// 安装程序和更新程序可根据此项目确定cfg项目的存储方式是cfg.pfs而非cfg文件目录。
/// 下面给出了一个.master.meta文件内容的示例：
///		<data v="1.0" o="1">
///			<f n="/cfg" h="hash of cfg/.files.meta" l="maybe 0"/>
///			<f n="/effect"/>
///			<f n="/map"/>
///			<f n="/pak"/>
///			<f n="/role"/>
///			<f n="/sound"/>
///			<f n="/ui"/>
///		</data>
///=============================================================================
/// Revision History :
/// 	V1.0 - 2009-10-16 Created.
///=============================================================================
#pragma once
#include <vector>
#include <string>
#include "pfs2.h"

namespace PFSX
{
	using PFS::FILE_DATA_HASH;

	const std::wstring MASTER_META_NAME = L"/.master.meta";
	const std::wstring SETUP_META_NAME = L"/.setup.meta";

	struct SetupMetaInfo // V1.0
	{
	private:
		std::wstring		m_Name;		// Name of setup pfs file
		FILE_DATA_HASH		m_Hash;		// Hash of .file.meta file
		FILE_DATA_HASH		m_Hash2;	// Hash of .file2.meta itself
		unsigned int		m_Flags;	// Flags of pfs file

	public:
		SetupMetaInfo()
			: m_Hash(0)
			, m_Hash2(0)
			, m_Flags(0)
		{
		}

		SetupMetaInfo(
			const std::wstring&		_Name,
			const FILE_DATA_HASH&	_Hash,
			const FILE_DATA_HASH&	_Hash2,
			unsigned int			_Flags)
			: m_Name(_Name)
			, m_Hash(_Hash)
			, m_Hash2(_Hash2)
			, m_Flags(_Flags)
		{
		}

		// 这里判断Name是不是同一个,即是不是同一个对象
		bool operator == ( const SetupMetaInfo& rOther ) const
		{
			// 这里只判断Name是不是同一个
			return ( this == &rOther ) || ( m_Name == rOther.m_Name );
		}

		// 这里判断对象的状态是否发生变化
		bool IsChanged( const SetupMetaInfo& rComparedTo ) const
		{
			if( this == &rComparedTo )
			{
				return false;
			}
			// assert( m_Name == rComparedTo.m_Name );
			return ( m_Name != rComparedTo.m_Name ||
				m_Hash != rComparedTo.m_Hash ||
				( m_Hash2 != 0 && rComparedTo.m_Hash2 != 0 && m_Hash2 != rComparedTo.m_Hash2 ) ||
				m_Flags != rComparedTo.m_Flags );
		}

		bool operator < ( const SetupMetaInfo& rOther ) const
		{
			return ( m_Name < rOther.m_Name );
		}

		SetupMetaInfo& Swap( SetupMetaInfo& rOther )
		{
			if( this != &rOther )
			{
				m_Name.swap( rOther.m_Name);
				std::swap( m_Hash, rOther.m_Hash );
				std::swap( m_Hash2, rOther.m_Hash2 );
				std::swap( m_Flags, rOther.m_Flags );
			}
			return *this;
		}

		const std::wstring& GetName() const { return m_Name; }
		const FILE_DATA_HASH& GetHash() const { return m_Hash; }
		const FILE_DATA_HASH& GetHash2() const { return m_Hash2; }
		unsigned int GetFlags() const { return m_Flags; }

		std::wstring& GetName() { return m_Name; }
		FILE_DATA_HASH& GetHash() { return m_Hash; }
		FILE_DATA_HASH& GetHash2() { return m_Hash2; }
		unsigned int& GetFlags() { return m_Flags; }

		void SetName(const std::wstring& _Name) { m_Name = _Name; }
		void SetHash(const FILE_DATA_HASH& _Hash) { m_Hash = _Hash; }
		void SetHash2(const FILE_DATA_HASH& _Hash2) { m_Hash2 = _Hash2; }
		void SetFlags(unsigned int _Flags) { m_Flags = _Flags; }
	};

	typedef std::vector<std::wstring> SetupPathVector;

	class SetupMetaVector : public std::vector<SetupMetaInfo>
	{
	public:
#pragma pack(push, 1)
		union
		{
			unsigned short m_wVersion; // 版本信息
			struct 
			{
				unsigned char m_bMinor;
				unsigned char m_bMajor;
			};
		};
#pragma pack(pop)

		bool m_bOriginal;	// 原始Meta标志
							// 老版本的更新程序直接从服务器下载此文件后,
							// 如未经处理而直接转换/copy,此标志仍在,
							// 更新程序据此认为是该文件是从老版本setupmeta过渡过来的,
							// 此时需要对所有子包进行处理,而不进行hash对比.

	public:
		SetupMetaVector()
		{
			m_wVersion = MAKEWORD(0,1); // 现在总是为1.0
			m_bOriginal = false;
		}

		bool IsOriginal() const 
		{
			return m_bOriginal;
		}

		void SetOriginal(bool bValue)
		{
			m_bOriginal = bValue;
		}
	};

	class CSetupMetaFile
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		//
		// default methods, allow overridden.
		//
		// file - pfs file object to load from or save to
		// data - memory block object to output to
		// metas - pfs file meta list to save to or to be saved
		// paths - pfs file name list to save to or to be saved
		//////////////////////////////////////////////////////////////////////////
		virtual bool Load( PFS::CBaseFile& file, SetupMetaVector& metas );
		virtual bool Load( PFS::CBaseFile& file, SetupPathVector& paths );
		virtual bool SaveAsData( PFS::CDataBlock& data, const SetupMetaVector& metas );
		virtual bool Save( PFS::CBaseFile& file, const SetupMetaVector& metas );

	public:
		//////////////////////////////////////////////////////////////////////////
		//
		// static methods - using the default reading/writing methods as declared above
		//
		// fileName - native file name or pfs file name to load from or save to
		// file - pfs file object to load from or save to
		// data - memory block object to output to
		// metas - pfs file name list to save to or to be saved
		//
		//////////////////////////////////////////////////////////////////////////
		static bool DoRead( const std::wstring& fileName, SetupMetaVector& metas )
		{
			PFS::CFile file;
			if( !file.Open( fileName, PFS::FM_EXCL, PFS::FA_RDONLY ) )
			{
				return false;
			}

			CSetupMetaFile reader;
			return reader.Load( file, metas );
		}
		
		static bool DoRead( PFS::CBaseFile& file, SetupMetaVector& metas)
		{
			CSetupMetaFile reader;
			return reader.Load( file, metas );
		}

		static bool DoRead( const std::wstring& fileName, SetupPathVector& paths )
		{
			PFS::CFile file;
			if( !file.Open( fileName, PFS::FM_EXCL, PFS::FA_RDONLY ) )
			{
				return false;
			}

			CSetupMetaFile reader;
			return reader.Load( file, paths );
		}

		static bool DoWrite( const std::wstring& fileName, const SetupMetaVector& metas )
		{
			PFS::CFile file;
			if( !file.Open( fileName, PFS::FM_CREAT|PFS::FM_TRUNC, PFS::FA_WRONLY ) )
			{
				return false;
			}

			CSetupMetaFile writer;
			return writer.Save( file, metas );
		}

		static bool DoWrite( PFS::CDataBlock& data, const SetupMetaVector& metas)
		{
			CSetupMetaFile writer;
			return writer.SaveAsData( data, metas );
		}

		static bool DoWrite( PFS::CBaseFile& file, const SetupMetaVector& metas)
		{
			CSetupMetaFile writer;
			return writer.Save( file, metas );
		}
	};

	///-----------------------------------------------------------------------------
	/// summary : 对2组setupmeta信息计算差异列表，用于计算差异清单，确定更新列表
	/// params  : rMinuend - 被减数
	///           rSubtrahend - 减数 
	///           rDifference -  差
	///-----------------------------------------------------------------------------
	void SubtractSetupMeta( const SetupMetaVector& rMinuend, 
		const SetupMetaVector& rSubtrahend, SetupMetaVector& rDifference );

	///-----------------------------------------------------------------------------
	/// summary : 对2组setupmeta信息计算集合元素排除列表，用于计算差异清单
	///           应用场景：本地集合rFullSet比服务器集合rSubSet存在更多元素，
	///           计算这些需要删除的本地集合中定义的元素。
	///           此计算不考虑那些存在、但属性有差异的元素。
	/// params  : rFullSet - 全集，已经按name排序。
	///           rSubSet - 子集，已经按name排序。
	///           rDifference - 差集
	///-----------------------------------------------------------------------------
	void DiffSetupMeta( const SetupMetaVector& rFullSet, 
		const SetupMetaVector& rSubSet, SetupMetaVector& rDifference );

	///-----------------------------------------------------------------------------
	/// summary : 从一组setupmeta中删除指定的元素。
	///           应用场景：本地集合rSet比服务器集合存在更多元素，这些元素为rToPergedSet，
	///           删除这些元素。rSet将被更新。
	///           结合DiffSetupMeta()使用，rToPergedSet应该为DiffSetupMeta返回的rDifference。
	/// params  : rSet - 全集
	///           rSetToPurge - 被删除的集合
	///-----------------------------------------------------------------------------
	void PurgeSetupMeta(SetupMetaVector& rSet, const SetupMetaVector& rSetToPurge);

	// 将 src 的项合并进 des
	void MergeSetupMeta( const SetupMetaVector& src, SetupMetaVector& des);

} // namespace PFSX