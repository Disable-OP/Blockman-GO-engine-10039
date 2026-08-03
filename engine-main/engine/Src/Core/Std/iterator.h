#ifndef __ITERATOR_HEADER__
#define __ITERATOR_HEADER__

#include <iterator>
#include "LibrarySupport.h"

#ifdef _MSC_VER 
	#if _MSC_VER <1800
		#if STD_LIBRARY_SUPPORT >= 14
			#include "cpp14/iterator.h"
		#endif
		#if STD_LIBRARY_SUPPORT >= 17
			#include "cpp17/iterator.h"
		#endif
	#endif
#else
	#if STD_LIBRARY_SUPPORT >= 14
		#include "cpp14/iterator.h"
	#endif
	#if STD_LIBRARY_SUPPORT >= 17
		#include "cpp17/iterator.h"
	#endif
#endif

#endif
