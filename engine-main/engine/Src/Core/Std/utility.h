#ifndef __UTILITY_HEADER__
#define __UTILITY_HEADER__

#include <utility>
#include "LibrarySupport.h"

#ifdef _MSC_VER 
#if _MSC_VER <1800
#if STD_LIBRARY_SUPPORT >= 14
#include "cpp14/utility.h"
#endif
//#if STD_LIBRARY_SUPPORT >= 17
//#include "cpp17/utility.h"
//#endif
#endif
#else
#if STD_LIBRARY_SUPPORT >= 14
#include "cpp14/utility.h"
#endif
//#if STD_LIBRARY_SUPPORT >= 17
//#include "cpp17/utility.h"
//#endif
#endif


#endif
