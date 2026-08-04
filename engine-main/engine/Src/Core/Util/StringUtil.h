/********************************************************************
filename: 	StringUtil.h
file path:	dev\engine\Src\Core\Util\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_STRINGUTIL_H__
#define __LORD_STRINGUTIL_H__

#include "Core.h"

namespace LORD
{
	class Vector2;
	class Vector3;
	class Vector4;
	class Color;
	class URect;
	class LORD_CORE_API StringUtil
	{
	private:
		StringUtil();
		~StringUtil();

	public:
		static const String		BLANK;

	public:
		static bool				Find(const String& str, const String& src);
		static String			Replace(const String& str, const String& src, const String& dst);
		static String			RemoveCharactors(const String& str, char c);
		static String			Replace(const String& str, char src, char dst);
		static bool				ReplaceRet(String& str, const String& src, const String& dst);
		static void				Trim(String& str, bool bLeft = true, bool bRight = true, const String& whitespaces = " \t\r\n");
		static StringArray		Split(const String& str, const String& delims = ", ", Dword maxSplits = 0, bool allowEmpty = false);
		static WStringArray		Split(const WString& str, const WString& delims = L",", Dword maxSplits = 0);
		static void				LowerCase(String& str);
		static void				UpperCase(String& str);
		static String			Format(const char* formats, ...);
		static bool				StartWith(const String& str, const String& pattern, bool lowCase = false);
		static bool				EndWith(const String& str, const String& pattern);
		static bool				Equal(const String& str1, const String& str2, bool bCaseSensitive = true);

		static String			WCS2MBS(const WString& str);
		static WString			MBS2WCS(const String& str);

		static String			ToString(float val, Word precision = 6, Word width = 0, char fill = ' ');
		static String			ToString(double val, Word precision = 6, Word width = 0, char fill = ' ');
		static String			ToString(int val, Word width = 0, char fill = ' ');
		static String			ToString(long val, Word width = 0, char fill = ' ');
		static String			ToString(unsigned int val, Word width = 0, char fill = ' ');
		static String			ToString(unsigned long val, Word width = 0, char fill = ' ');
		static String			ToString(long long val, Word width = 0, char fill = ' ');
		static String			ToString(bool val, bool bYesNo = false);
		static String			ToString(const Vector2& val);
		static String			ToString(const Vector3& val);
		static String			ToString(const Vector4& val);
		static String			ToString(const Quaternion& val);
		static String			ToString(const Color& val);
		static String			ToThousandthString(int val);

#ifdef LORD_INCLUDE_GUI
		static String			ToString(const URect& val);
#endif

		static float			ParseFloat(const String& val, float defVal = 0.0f);
		static double			ParseDouble(const String& val, double defVal = 0.0);
		static Real				ParseReal(const String& val, Real defVal = (Real)0.0);
		static int				ParseInt(const String& val, int defVal = 0);
		static long				ParseLong(const String& val, long defVal = 0);
		static bool				ParseBool(const String& val, bool defVal = false);
		static Word				ParseWord(const String& val, Word defVal = 0xffff);
		static Dword			ParseDword(const String& val, Dword defVal = 0xffffffff);
		static i8				ParseI8(const String& val, i8 defVal = 0);
		static i16				ParseI16(const String& val, i16 defVal = 0);
		static i32				ParseI32(const String& val, i32 defVal = 0);
		static i64				ParseI64(const String& val, i64 defVal = 0);
		template<typename T>
		static T				ParseHex(const String& val, T defaultValue = 0)
		{
			// Use istringstream for direct correspondence with to_string
			StringStream ss;
			ss << std::hex;
			ss.str(val);
			ss >> defaultValue;
			return defaultValue;
		}
		static ui8				ParseUI8(const String& val, ui8 defVal = 0);
		static ui16				ParseUI16(const String& val, ui16 defVal = 0);
		static ui32				ParseUI32(const String& val, ui32 defVal = 0);
		static ui64				ParseUI64(const String& val, ui64 defVal = 0);
		static Vector2			ParseVec2(const String& val);
		static Vector3			ParseVec3(const String& val);
		static Vector4			ParseVec4(const String& val);
		static Quaternion		ParseQuaternion(const String& val);
		static Color			ParseColor(const String& val);
		static Color			ParseColor255(const String& val);
#ifdef LORD_INCLUDE_GUI
		static URect			ParseURect(const String& val);
#endif

		static bool				IsNumber(const String& val);
		static String			Hex2Char(Dword val);

		static void				GBKToUTF8(char* dst, uint& dstLen, const char* src, uint srcLen);
		static void				UTF8ToGBK(char* dst, uint& dstLen, const char* src, uint srcLen);
		static void				UTF8ToUTF32(UTF32String& utf32Str, const UTF8String& utf8Str);
		static void				UTF32ToUTF8(UTF8String& utf8Str, const UTF32String& utf32Str);

		template<typename ... Args>
		static String concatToString(Args&& ... args) {
			StringStream ss;
			innerConcatToString(ss, args...);
			return ss.str();
		}

	private:
		template<typename T, typename ... Args>
		static void innerConcatToString(StringStream &ss, T&& arg, Args&& ... args) {
			ss << arg;
			innerConcatToString(ss, args...);
		}

		template<typename T>
		static void innerConcatToString(StringStream &ss, T&& arg) {
			ss << arg;
		}
	};
}

#endif
