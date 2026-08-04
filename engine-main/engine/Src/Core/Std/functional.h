#ifndef __FUNCTIONAL_HEADER__
#define __FUNCTIONAL_HEADER__

#include <functional>
#include "LibrarySupport.h"

#ifdef _MSC_VER 
#if _MSC_VER <1800
//#if STD_LIBRARY_SUPPORT >= 14
//#include "cpp14/functional.h"
//#endif
#if STD_LIBRARY_SUPPORT >= 17
#include "cpp17/functional.h"
#endif
#endif
#else
//#if STD_LIBRARY_SUPPORT >= 14
//#include "cpp14/functional.h"
//#endif
#if STD_LIBRARY_SUPPORT >= 17
#include "cpp17/functional.h"
#endif
#endif

#endif
