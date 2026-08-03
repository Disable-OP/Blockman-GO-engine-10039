#include "Core.h"
#include "StringUtil.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quaternion.h"
#include "Image/Color.h"
#include "Encoder.h"
#ifdef LORD_INCLUDE_GUI
#include "UI/GUIUDim.h"
#endif
#include <stdarg.h>
//#include "Util/AssertX.h"

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#endif

namespace LORD
{

	static char _HexToChar[] =
	{
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};

	const String StringUtil::BLANK;

	StringUtil::StringUtil()
	{
	}

	StringUtil::~StringUtil()
	{
	}

	bool StringUtil::Find(const String & str, const String & src)
	{
		String out = str;
		size_t pos = str.find(src, 0);
		return pos != String::npos;
	}

	String StringUtil::Replace(const String& str, const String& src, const String& dst)
	{    
		if (src == dst)
			return str;

		String out = str;
		size_t pos = str.find(src, 0);
		while (pos != String::npos)
		{
			out.replace(pos, src.size(), dst);
			pos = out.find(src, pos+dst.size());
		}

		return out;
	}

	String StringUtil::RemoveCharactors(const String& str, char c)
	{
		size_t s = str.size();
		char* ret = (char*)LordMalloc(s+1);
		const char* src = str.c_str();
		size_t i,j;
		for(i=0,j=0; i<s; ++i)
		{
			if(src[i] != c)
				ret[j++] = src[i];
		}
		ret[j] = 0;
		String rets = ret;
		LordFree(ret);
		return rets;
	}

	String StringUtil::Replace(const String& str, char src, char dst)
	{
		String out = str;
		char* buf = &(*out.begin());
		while (*buf)
		{
			if (*buf == src)
				*buf = dst;
			buf++;
		}

		return out;
	}

	bool StringUtil::ReplaceRet(String& str, const String& src, const String& dst)
	{    
		if (src == dst)
			return false;

		bool bReplaced = false;
		size_t pos = str.find(src, 0);

		if(pos != String::npos)
		{
			str.replace(pos, src.size(), dst);
			bReplaced = true;
		}

		return bReplaced;
	}

	void StringUtil::Trim(String& str, bool bLeft, bool bRight, const String& whitespaces)
	{
		/*
		size_t lspaces, rspaces, len = length(), i;

		lspaces = rspaces = 0;

		if( left )
		{
		// Find spaces / tabs on the left
		for( i = 0;
		i < len && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
		++lspaces, ++i );
		}

		if( right && lspaces < len )
		{
		// Find spaces / tabs on the right
		for( i = len - 1;
		i >= 0 && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
		rspaces++, i-- );
		}

		*this = substr(lspaces, len-lspaces-rspaces);
		*/
		if(bRight)
			str.erase(str.find_last_not_of(whitespaces)+1); // trim right
		if(bLeft)
			str.erase(0, str.find_first_not_of(whitespaces)); // trim left
	}

	StringArray StringUtil::Split(const String& str, const String& delims, Dword maxSplits, bool allowEmpty)
	{
		StringArray ret;

		if(str.empty())
			return ret;

		// Pre-allocate some space for performance
		ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

		Dword numSplits = 0;

		// Use STL methods
		size_t start, pos;
		start = 0;
		do
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				// Do nothing
				if(allowEmpty)
					ret.push_back(""); // ÔÊÐí¿Õ×Ó´®

				start = pos + 1;
			}
			else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the string
				ret.push_back( str.substr(start) );
				break;
			}
			else
			{
				// Copy up to delimiter
				ret.push_back( str.substr(start, pos - start) );
				start = pos + 1;
			}
			// parse up to next real data
			start = str.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != String::npos);

		return ret;
	}

	WStringArray StringUtil::Split(const WString& str, const WString& delims, Dword maxSplits)
	{
		WStringArray ret;

		if (str.empty())
			return ret;

		// Pre-allocate some space for performance
		ret.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

		Dword numSplits = 0;

		// Use STL methods
		size_t start, pos;
		start = 0;
		do
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				// Do nothing
				start = pos + 1;
			}
			else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
			{
				// Copy the rest of the string
				ret.push_back(str.substr(start));
				break;
			}
			else
			{
				// Copy up to delimiter
				ret.push_back(str.substr(start, pos - start));
				start = pos + 1;
			}
			// parse up to next real data
			start = str.find_first_not_of(delims, start);
			++numSplits;

		} while (pos != WString::npos);

		return ret;
	}

	void StringUtil::LowerCase(String& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) tolower);
	}

	void StringUtil::UpperCase(String& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), (int(*)(int)) toupper);
	}

	String StringUtil::Format(const char* formats, ... )
	{
		char szBuffer[4096];

		va_list args;
		va_start(args, formats);
		vsprintf(szBuffer, formats, args);
		va_end(args);

		return szBuffer;
	}

	bool StringUtil::StartWith(const String& str, const String& pattern, bool lowCase)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		String startOfThis = str.substr(0, patternLen);
		if(lowCase)
			StringUtil::LowerCase(startOfThis);

		return (startOfThis == pattern);
	}

	bool StringUtil::EndWith(const String& str, const String& pattern)
	{
		size_t thisLen = str.length();
		size_t patternLen = pattern.length();
		if (thisLen < patternLen || patternLen == 0)
			return false;

		String endOfThis = str.substr(thisLen - patternLen, patternLen);

		return (endOfThis == pattern);
	}

	bool StringUtil::Equal(const String& str1, const String& str2, bool bCaseSensitive/*= true*/)
	{
		if (bCaseSensitive)
		{
			return (str1 == str2);
		}
		else
		{
			String lstr1 = str1;
			String lstr2 = str2;
			LowerCase(lstr1);
			LowerCase(lstr2);
			return (lstr1 == lstr2);
		}
	}

	String StringUtil::WCS2MBS(const WString &str)
	{	
#	if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		int size = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
#	else
		size_t size = wcstombs(NULL, str.c_str(), 0);
#	endif

		String mbstr(size, wchar_t(0));
#	if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, const_cast<char*>(mbstr.c_str()), size, NULL, NULL);
#	else
		wcstombs(const_cast<char*>(mbstr.c_str()), const_cast<wchar_t*>(str.c_str()), (size+1)*4);
#	endif

		return mbstr;
	}

	WString StringUtil::MBS2WCS(const String& str)
	{
#	if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0)-1;
#	else
		size_t size = mbstowcs(NULL, str.c_str(), 0);
#	endif

		WString wstr(size, wchar_t(0));
#	if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.length(), const_cast<wchar_t*>(wstr.c_str()), size);
#	else
		mbstowcs(const_cast<wchar_t*>(wstr.c_str()), str.c_str(), size);
#	endif
		return wstr;
	}

	String StringUtil::ToString(float val, Word precision, Word width, char fill)
	{
		StringStream ss;
		ss.precision(precision);
		ss.width(width);
		ss.fill(fill);
		ss << val;
		return ss.str();
	}

	String StringUtil::ToString(double val, Word precision, Word width, char fill)
	{
		StringStream ss;
		ss.precision(precision);
		ss.width(width);
		ss.fill(fill);
		ss << val;
		return ss.str();
	}

	String StringUtil::ToString(int val, Word width, char fill)
	{
		StringStream ss;
		ss.width(width);
		ss.fill(fill);
		ss << val;
		return ss.str();
	}

	String StringUtil::ToString(long long val, Word width, char fill)
	{
		StringStream ss;
		ss.width(width);
		ss.fill(fill);
		ss << val;
		return ss.str();
	}

	String StringUtil::ToString(long val, Word width, char fill)
	{
		StringStream ss;
		ss.width(width);
		ss.fill(fill);
		ss << val;
		return ss.str();
	}

	String StringUtil::ToString(unsigned int val, Word width, char fill)
	{
		StringStream ss;
		ss.width(width);
		ss.fill(fill);
		ss << val;
		return ss.str();
	}

	String StringUtil::ToString(unsigned long val, Word width, char fill)
	{
		StringStream ss;
		ss.width(width);
		ss.fill(fill);
		ss << val;
		return ss.str();
	}

	String StringUtil::ToString(bool val, bool bYesNo)
	{
		if (val)
		{
			if (bYesNo)
				return "yes";
			else
				return "true";
		}
		else
		{
			if (bYesNo)
				return "no";
			else
				return "false";
		}
	}

	String StringUtil::ToString(const Vector2& val)
	{
		StringStream stream;
		stream << val.x << " " << val.y;
		return stream.str();
	}

	String StringUtil::ToString(const Vector3& val)
	{
		StringStream stream;
		stream << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}

	String StringUtil::ToString(const Vector4& val)
	{
		StringStream stream;
		stream << val.x << " " << val.y << " " << val.z << " " << val.w;
		return stream.str();
	}

	String StringUtil::ToString(const Quaternion& val)
	{
		StringStream stream;
		stream << val.w << " " << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}

	String StringUtil::ToString(const Color& val)
	{
		StringStream stream;
		stream << val.r << " " << val.g << " " << val.b << " " << val.a;
		return stream.str();
	}

	String StringUtil::ToThousandthString(int val)
	{
		bool isNegetive = val < 0;
		String ret = ToString(isNegetive ? -val : val);
		auto size = ret.size();
		while (size > 3)
		{
			ret = ret.insert(size - 3, 1, ',');
			size -= 3;
		}

		if (isNegetive)
			ret = ret.insert(0, 1, '-');
		
		return ret;
	}

#ifdef LORD_INCLUDE_GUI
	String StringUtil::ToString(const URect& val)
	{

		String str = Format("{{%g,%g},{%g,%g},{%g,%g},{%g,%g}}", 
			val.d_min.d_x.d_scale, val.d_min.d_x.d_offset,
			val.d_min.d_y.d_scale, val.d_min.d_y.d_offset,
			val.d_max.d_x.d_scale, val.d_max.d_x.d_offset,
			val.d_max.d_y.d_scale, val.d_max.d_y.d_offset );

		return str;
	}
#endif


	float StringUtil::ParseFloat(const String& val, float defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		float ret = defVal;
		str >> ret;

		return ret;
	}

	double StringUtil::ParseDouble(const String& val, double defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		double ret = defVal;
		str >> ret;

		return ret;
	}

	Real StringUtil::ParseReal(const String& val, Real defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		Real ret = defVal;
		str >> ret;

		return ret;
	}

	int StringUtil::ParseInt(const String& val, int defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		int ret = defVal;
		str >> ret;

		return ret;
	}

	long StringUtil::ParseLong(const String& val, long defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		long ret = defVal;
		str >> ret;

		return ret;
	}

	bool StringUtil::ParseBool(const String& val, bool defVal)
	{
		if (StringUtil::Equal(val, "true", false) ||
			StringUtil::Equal(val, "yes", false) ||
			StringUtil::Equal(val, "1", false))
			return true;
		else if(StringUtil::Equal(val, "false", false) ||
			StringUtil::Equal(val, "no", false) ||
			StringUtil::Equal(val, "0", false))
			return false;
		else
			return defVal;
	}

	Word StringUtil::ParseWord(const String& val, Word defVal /*= 0xffff*/)
	{
		StringStream ss(val.c_str());
		Word ret = defVal;
		ss >> std::hex >> ret;
		return ret;
	}

	Dword StringUtil::ParseDword(const String& val, Dword defVal /*= 0xffffffff*/)
	{
		StringStream ss(val.c_str());
		Dword ret = defVal;
		ss >> std::hex >> ret;
		return ret;
	}

	i8 StringUtil::ParseI8(const String& val, i8 defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		i8 ret = defVal;
		str >> ret;

		return ret;
	}

	i16 StringUtil::ParseI16(const String& val, i16 defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		i16 ret = defVal;
		str >> ret;

		return ret;
	}

	i32 StringUtil::ParseI32(const String& val, i32 defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		i32 ret = defVal;
		str >> ret;

		return ret;
	}

	i64 StringUtil::ParseI64(const String& val, i64 defaultValue)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		i64 ret = defaultValue;
		str >> ret;

		return ret;
	}

	ui8 StringUtil::ParseUI8(const String& val, ui8 defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		ui8 ret = defVal;
		str >> ret;

		return ret;
	}

	ui16 StringUtil::ParseUI16(const String& val, ui16 defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		ui16 ret = defVal;
		str >> ret;

		return ret;
	}

	ui32 StringUtil::ParseUI32(const String& val, ui32 defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		ui32 ret = defVal;
		str >> ret;

		return ret;
	}

	ui64 StringUtil::ParseUI64(const String& val, ui64 defVal)
	{
		// Use istringstream for direct correspondence with to_string
		StringStream str(val);
		ui64 ret = defVal;
		str >> ret;

		return ret;
	}

	Vector2 StringUtil::ParseVec2(const String& val)
	{
		// Split on space
		StringArray vec = StringUtil::Split(val);

		if (vec.size() != 2)
		{
			return Vector2::ZERO;
		}
		else
		{
			return Vector2(ParseReal(vec[0]), ParseReal(vec[1]));
		}
	}

	Vector3 StringUtil::ParseVec3(const String& val)
	{
		StringArray vec = StringUtil::Split(val);

		if(vec.size() != 3)
		{
			return Vector3::ZERO;
		}
		else
		{
			return Vector3(ParseReal(vec[0]), ParseReal(vec[1]), ParseReal(vec[2]));
		}
	}

	Vector4 StringUtil::ParseVec4(const String& val)
	{
		StringArray vec = StringUtil::Split(val);

		if(vec.size() != 4)
		{
			return Vector4::ZERO;
		}
		else
		{
			return Vector4(ParseReal(vec[0]), ParseReal(vec[1]), ParseReal(vec[2]), ParseReal(vec[3]));
		}
	}

	LORD::Quaternion StringUtil::ParseQuaternion(const String& val)
	{
		StringArray vec = StringUtil::Split(val);
		if (vec.size() != 4)
		{
			return Quaternion::IDENTITY;
		}
		else
		{
			return Quaternion(ParseReal(vec[0]), ParseReal(vec[1]), ParseReal(vec[2]), ParseReal(vec[3]));
		}
	}

	Color StringUtil::ParseColor(const String& val)
	{
		StringArray vec = StringUtil::Split(val);

		if(vec.size() != 4)
		{
			return Color::BLACK;
		}
		else
		{
			return Color(ParseReal(vec[0]), ParseReal(vec[1]), ParseReal(vec[2]), ParseReal(vec[3]));
		}
	}

	Color StringUtil::ParseColor255(const String& val)
	{
		StringArray vec = StringUtil::Split(val);
		if (vec.size() == 3)
		{
			int r = ParseInt(vec[0]);
			int g = ParseInt(vec[1]);
			int b = ParseInt(vec[2]);
			r = Math::Clamp(r, 0, 255);
			g = Math::Clamp(g, 0, 255);
			b = Math::Clamp(b, 0, 255);
			return Color(r / 255.f, g / 255.f, b / 255.f, 1.f);
		}
		return Color::WHITE;
	}

#ifdef LORD_INCLUDE_GUI
	URect StringUtil::ParseURect(const String& val)
	{
		using namespace std;

		URect ur;
		sscanf(
			val.c_str(),
			" { { %g , %g } , { %g , %g } , { %g , %g } , { %g , %g } }",
			&ur.d_min.d_x.d_scale, &ur.d_min.d_x.d_offset,
			&ur.d_min.d_y.d_scale, &ur.d_min.d_y.d_offset,
			&ur.d_max.d_x.d_scale, &ur.d_max.d_x.d_offset,
			&ur.d_max.d_y.d_scale, &ur.d_max.d_y.d_offset
			);

		return ur;
	}
#endif

	bool StringUtil::IsNumber(const String& val)
	{
		StringStream str(val);
		float tst;
		str >> tst;
		return !str.fail() && str.eof();
	}

	String StringUtil::Hex2Char(Dword val)
	{
		String out;
		out.resize(4*2+1);// one byte - two characters

		char* to = (char*)out.c_str();
		Byte *from = (Byte*)(&val) + 3;

		for (int i = 0; i < 4; ++i)
		{
			*(to++) = _HexToChar[(*from) >> 4];		// 4 high bits.
			*(to++) = _HexToChar[(*from) & 0x0f];	// 4 low bits.

			--from;
		}

		return out;
	}

	void StringUtil::GBKToUTF8(char* dst, uint& dstLen, const char* src, uint srcLen)
	{
		char* *pdst = &dst;
		_GBK_TO_UTF8(src, srcLen, pdst, &dstLen);
	}

	void StringUtil::UTF8ToGBK(char* dst, uint& dstLen, const char* src, uint srcLen)
	{
		char* *pdst = &dst;
		_UTF8_TO_GBK(src, srcLen, pdst, &dstLen);
	}

	// utf-8 to utf-32 
	// return the utf-32 count
	size_t encoded_size(const unsigned char* buf, size_t len)
	{
		size_t count = 0;

		while (len--)
		{
			unsigned char tcp = *buf++;
			++count;
			size_t size = 0;

			if (tcp < 0x80)
			{
			}
			else if (tcp < 0xE0)
			{
				size = 1;
				++buf;
			}
			else if (tcp < 0xF0)
			{
				size = 2;
				buf += 2;
			}
			else
			{
				size = 3;
				buf += 3;
			}

			if (len >= size)
				len -= size;
			else 
				break;
		}

		return count;
	}

	size_t utf_length(const unsigned char* utf8_str)
	{
		size_t cnt = 0;
		while (*utf8_str++)
			cnt++;

		return cnt;
	}

	void StringUtil::UTF8ToUTF32(UTF32String& utf32Str, const UTF8String& utf8Str)
	{
		if (utf8Str.empty())
			return ;

		unsigned char* src = (unsigned char*)utf8Str.c_str();

		size_t nUTF32Len = encoded_size(src, utf_length(src));
		if (nUTF32Len == 0)
			return ;

		utf32* pTempBuffer = LordAlloc(utf32, nUTF32Len+1);
		utf32* pStartBuffer = pTempBuffer;
		pTempBuffer[nUTF32Len] = 0;

		// encode
		//----------------------------------------------------------------------

		size_t destCapacity = nUTF32Len;
		// while there is data in the source buffer, and space in the dest buffer
		for (uint idx = 0; (idx < utf8Str.length() && destCapacity > 0);)
		{
			utf32	cp;
			unsigned char cu = src[idx++];

			if (cu < 0x80)
			{
				cp = (utf32)(cu);
			}
			else if (cu < 0xE0)
			{
				cp = ((cu & 0x1F) << 6);
				cp |= (src[idx++] & 0x3F);
			}
			else if (cu < 0xF0)
			{
				cp = ((cu & 0x0F) << 12);
				cp |= ((src[idx++] & 0x3F) << 6);
				cp |= (src[idx++] & 0x3F);
			}
			else
			{
				cp = ((cu & 0x07) << 18);
				cp |= ((src[idx++] & 0x3F) << 12);
				cp |= ((src[idx++] & 0x3F) << 6);
				cp |= (src[idx++] & 0x3F);
			}

			*pTempBuffer++ = cp;
			--destCapacity;
		}

		// --------------------------------------------------------------------

		utf32Str = pStartBuffer;
		LordFree(pStartBuffer);
	}

	// -----utf 32 to utf 8
	// calculate the buff size utf8 required
	size_t utf_length(const utf32* utf32_str)
	{
		size_t cnt = 0;
		while (*utf32_str++)
			cnt++;

		return cnt;
	}

	// return the number of utf8 code units required to encode the given utf32 code point
	size_t encoded_size(utf32 code_point)
	{
		if (code_point < 0x80)
			return 1;
		else if (code_point < 0x0800)
			return 2;
		else if (code_point < 0x10000)
			return 3;
		else
			//return 4;
			return 0;
	}

	// return number of code units required to re-encode given utf32 data as utf8.   len is number of code units in 'buf'.
	size_t encoded_size(const utf32* buf, size_t len)
	{
		size_t count = 0;

		while (len--)
		{
			count += encoded_size(*buf++);
		}

		return count;
	}

	// return number of code units required to re-encode given null-terminated utf32 data as utf8.  return does not include terminating null.
	size_t encoded_size(const utf32* buf)
	{
		return encoded_size(buf, utf_length(buf));
	}

	void StringUtil::UTF32ToUTF8( UTF8String& utf8Str, const UTF32String& utf32Str )
	{
		if (utf32Str.empty())
		{
			return;
		}

		utf32* utf32buffer = (utf32*)utf32Str.c_str();
		size_t utf8Size = encoded_size(utf32buffer);
		size_t src_len = utf32Str.size();

		if (utf8Size == 0)
		{
			return;
		}

		utf8* pTempBuffer = LordAlloc(utf8, utf8Size+1);
		utf8* pStartBuffer = pTempBuffer;
		pTempBuffer[utf8Size] = 0;

		// while there is data in the source buffer,
		for (uint idx = 0; idx < src_len; ++idx)
		{
			utf32 cp = utf32buffer[idx];

			if (cp < 0x80)
			{
				*pTempBuffer++ = (utf8)cp;
			}
			else if (cp < 0x0800)
			{
				*pTempBuffer++ = (utf8)((cp >> 6) | 0xC0);
				*pTempBuffer++ = (utf8)((cp & 0x3F) | 0x80);
			}
			else if (cp < 0x10000)
			{
				*pTempBuffer++ = (utf8)((cp >> 12) | 0xE0);
				*pTempBuffer++ = (utf8)(((cp >> 6) & 0x3F) | 0x80);
				*pTempBuffer++ = (utf8)((cp & 0x3F) | 0x80);
			}
			else
			{
				*pTempBuffer++ = (utf8)((cp >> 18) | 0xF0);
				*pTempBuffer++ = (utf8)(((cp >> 12) & 0x3F) | 0x80);
				*pTempBuffer++ = (utf8)(((cp >> 6) & 0x3F) | 0x80);
				*pTempBuffer++ = (utf8)((cp & 0x3F) | 0x80);
			}
		}

		utf8Str = (char*)pStartBuffer;
		LordFree(pStartBuffer);
	}
}
