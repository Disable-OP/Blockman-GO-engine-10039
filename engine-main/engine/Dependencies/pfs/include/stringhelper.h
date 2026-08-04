///=============================================================================
/// Product   : PFS
/// Project   : Miscellaneous Files
/// Module    : stringhelper.h
/// Copyright : (c) 2009 WanMei, Inc. All rights reserved.
/// Creator   : liudongxue
///=============================================================================
/// Description :
/// 	CStringHelper
///		1. std::wstring 和其它数据类型的相互转换函数
///=============================================================================
/// Revision History :
/// 	V1.0 - 2009-10-27 Created.
///=============================================================================
#pragma once
#include "pfs2.h"

namespace PFSX
{
	class CStringHelper
	{
	private:
		CStringHelper() {}
		~CStringHelper() {}
	public:
		static const std::wstring TABLE;
		static wchar_t HexToInt( wchar_t hexChar )
		{
			if( hexChar >= L'0' && hexChar <= L'9' )
			{
				return (wchar_t)(hexChar - L'0');
			}
			else if( hexChar >= L'a' && hexChar <= L'f' )
			{
				return (wchar_t)(hexChar - L'a' + 10 );
			}
			else if( hexChar >= L'A' && hexChar <= L'F' )
			{
				return (wchar_t)(hexChar - L'A' + 10 );
			}
			else
			{
				return 0;
			}
		}
		static PFS::CDataBlock String2Data( const std::wstring& str)
		{
			PFS::CDataBlock data;
			const wchar_t* pwstr = str.c_str();
			size_t len = str.size();

			// str必须保证为偶数长度字符数，否则无法保证正确性
			assert( len % 2 == 0 );

			data.Resize( (len + 1) /2 );
			char* pstr = (char*)data.GetData();
			size_t j = 0;
			if ( len %2 != 0 )
			{
				len--;
				for( size_t i=0; i<len; i+=2)
				{
					pstr[j++] = (char)(((unsigned int)HexToInt(pwstr[i]) << 4) + HexToInt(pwstr[i+1]));
				}

				pstr[j++] = (char) HexToInt(pwstr[len]);
			}
			else
			{
				for( size_t i=0; i<len; i+=2)
				{
					pstr[j++] = (char)(((unsigned int)HexToInt(pwstr[i]) << 4) + HexToInt(pwstr[i+1]));
				}
			}

			data.Resize( j );
			return data;
		}
		static int String2Int( const std::wstring& str) { return _wtoi( str.c_str()); }

		static PFS::fsize_t String2FileSize( const std::wstring& str) { return _wtoi64( str.c_str()); }

		static std::wstring Data2String( const PFS::CDataBlock& src)
		{
			std::wstring str;

			const char* data = (const char*)src.GetData();
			size_t count = src.GetSize();
			for( size_t i = 0; i < count; i ++)
			{
				const char& c = data[i];
				str += TABLE[ (c >> 4) & 0x0f];
				str += TABLE[ c & 0x0f];
			}
			return str;
		}
		static std::wstring Int2String( int n)
		{
			const size_t	size = 16;
			wchar_t		str[size];
			_itow_s( n, str, size, 10);
			return str;
		}

		static std::wstring FileSize2String( fsize_t fsize)
		{
			const size_t	size = 16;
			wchar_t		str[size];
			_i64tow_s( fsize, str, size, 10);
			return str;
		}

		static PFS::FILE_DATA_HASH String2Hash( const std::wstring& strHash )
		{
			// HashString必须是二进制的8字节hash值转换为16进制表示形式的字符串
			assert( strHash.size() == sizeof(PFS::FILE_DATA_HASH)*2 );

			PFS::FILE_DATA_HASH hash = 0;
			PFS::CDataBlock data = String2Data( strHash );
			memcpy( &hash, data.GetData(), sizeof( PFS::FILE_DATA_HASH));
			return hash;
		}

		static std::wstring Hash2String( PFS::FILE_DATA_HASH hash)
		{
			PFS::CDataBlock data;
			data.Append(&hash, sizeof( PFS::FILE_DATA_HASH));
			std::wstring strHash = Data2String( data );
			return strHash;
		}
	};
} // namespace PFSX