#ifndef __SHARED_MUTEX_HEADER__
#define __SHARED_MUTEX_HEADER__

#include "LibrarySupport.h"

#ifdef _MSC_VER 
#	if STD_LIBRARY_SUPPORT >= 17
#		include <shared_mutex>
#	else
#		include "cpp17/shared_mutex.h"
#	endif
#else
#	if STD_LIBRARY_SUPPORT >= 17
#		include "cpp17/shared_mutex.h"
#	endif
#endif

#endif // !__SHARED_MUTEX_HEADER__
