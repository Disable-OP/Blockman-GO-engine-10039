#ifndef __STRING_VIEW_HEADER__
#define __STRING_VIEW_HEADER__

#include "LibrarySupport.h"

#ifdef _MSC_VER 
	#if !_HAS_CXX17
		#if STD_LIBRARY_SUPPORT >= 17
			#include "cpp17/string_view.h"
		#endif
	#else
		#include <string_view>
	#endif
#else
	#if STD_LIBRARY_SUPPORT >= 17
		#include "cpp17/string_view.h"
	#endif
#endif

#endif
