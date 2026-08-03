#ifndef __CHRONO_HEADER__
#define __CHRONO_HEADER__

#include <chrono>
#include "LibrarySupport.h"

#ifdef _MSC_VER 
#if _MSC_VER <1800
#if STD_LIBRARY_SUPPORT >= 14
#include "cpp14/chrono.h"
#endif
#if STD_LIBRARY_SUPPORT >= 17
#include "cpp17/chrono.h"
#endif
#endif
#else
#if STD_LIBRARY_SUPPORT >= 14
#include "cpp14/chrono.h"
#endif
#if STD_LIBRARY_SUPPORT >= 17
#include "cpp17/chrono.h"
#endif
#endif

#endif
