#ifndef __TYPE_TRAITS_HEADER__
#define __TYPE_TRAITS_HEADER__

#include <type_traits>
#include "LibrarySupport.h"

#ifdef _MSC_VER 
	#if _MSC_VER <1800
		#if STD_LIBRARY_SUPPORT >= 14
			#include "cpp14/type_traits.h"
		#endif
		#if STD_LIBRARY_SUPPORT >= 17
			#include "cpp17/type_traits.h"
		#endif
	#endif
#else
	#if STD_LIBRARY_SUPPORT >= 14
		#include "cpp14/type_traits.h"
	#endif
	#if STD_LIBRARY_SUPPORT >= 17
		#include "cpp17/type_traits.h"
	#endif
#endif


#endif
