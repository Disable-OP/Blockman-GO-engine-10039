/********************************************************************
filename: 	TypeDef.h
file path:	dev\engine\Src\Core\Common\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_TYPEDEF_H__
#define __LORD_TYPEDEF_H__
#include <cstdint>
#include "CoreDef.h"

namespace LORD
{
	typedef std::int8_t			i8;			//!< ¨C128 to 127
	typedef std::int16_t		i16;		//!< ¨C32,768 to 32,767
	typedef std::int32_t		i32;		//!< ¨C2,147,483,648 to 2,147,483,647
	typedef std::int64_t		i64;		//!< ¨C9,223,372,036,854,775,808 to 9,223,372,036,854,775,807
	typedef std::uint8_t		ui8;
	typedef std::uint16_t		ui16;
	typedef std::uint32_t		ui32;
	typedef std::uint64_t		ui64;

	typedef unsigned int		uint;
	typedef unsigned long		ulong;
	typedef float				real32;		//!< .4E +/- 38 (7 digits)
	typedef double				real64;		//!< 1.7E +/- 308 (15 digits)
	typedef ui8					bool8;

	typedef unsigned int		Dword;
	typedef bool				Bool;
	typedef unsigned char		Byte;
	typedef unsigned short		Word;

#if (LORD_PRECISION == LORD_PREC_DOUBLE)
	typedef double			Real;
#else
	typedef float			Real;
#endif

	// STL Type Define
#if ((LORD_COMPILER == LORD_COMPILER_GNUC) && (LORD_COMPILER_VERSION >= 310) && !defined(STLPORT))
#   if LORD_COMPILER_VERSION >= 430
#       define HashMap	::std::tr1::unordered_map
#		define HashSet	::std::tr1::unordered_set
#		define HashCode	::std::tr1::hash
#    else
#       define HashMap	::__gnu_cxx::hash_map
#       define HashSet	::__gnu_cxx::hash_set
#       define HashCode	::std::hash
#    endif
#elif LORD_COMPILER == LORD_COMPILER_CLANG
#    if defined(_LIBCPP_VERSION)
#       define HashMap	::std::unordered_map
#       define HashSet	::std::unordered_set
#       define HashCode	::std::hash
#    else
#       define HashMap	::std::tr1::unordered_map
#       define HashSet	::std::tr1::unordered_set
#       define HashCode	::std::tr1::hash
#    endif
#else
#   if LORD_COMPILER == LORD_COMPILER_MSVC
#       if LORD_COMPILER_VERSION >= 100 // VC++ 10.0
#			define HashMap	::std::tr1::unordered_map
#           define HashSet	::std::tr1::unordered_set
#			define HashCode	::std::tr1::hash
#		elif LORD_COMPILER_VERSION > 70 && !defined(_STLP_MSVC)
#           define HashMap	::stdext::hash_map
#           define HashSet	::stdext::hash_set
#           define HashCode	::stdext::hash
#       else
#           define HashMap	::std::hash_map
#           define HashSet	::std::hash_set
#           define HashCode	::std::hash
#		endif
#	else
#		define HashMap	::std::hash_map
#		define HashSet	::std::hash_set
#		define HashCode	::std::hash
#	endif
#endif

}


#endif
