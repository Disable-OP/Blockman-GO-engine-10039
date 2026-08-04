/********************************************************************
filename: 	AssertX.h
file path:	dev\engine\Src\Core\Util\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ASSERTX_H__
#define __LORD_ASSERTX_H__

#include "StringUtil.h"

namespace LORD
{

	// Assert function return values
	enum ErrRet
	{
		ERRRET_IGNORE = 0,
		ERRRET_CONTINUE,
		ERRRET_BREAKPOINT,
		ERRRET_ABORT
	};

	//- Global functions ----------------------------------------------------------
	LORD_CORE_API ErrRet NotifyAssert(const char* condition, const char* fileName, int lineNumber, const char* formats, ...);


#if (LORD_MODE == LORD_MODE_DEBUG)
#	if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#		define LordAssertX(x, formats, ...) { \
	static bool _ignoreAssert = false; \
	if (!_ignoreAssert && !(x)) \
	{ \
	ErrRet _err_result = NotifyAssert(#x, __FILE__, __LINE__, formats, ##__VA_ARGS__); \
	if (_err_result == ERRRET_IGNORE) \
	{ \
	_ignoreAssert = true; \
	} \
	else if (_err_result == ERRRET_BREAKPOINT) \
	{ \
	__debugbreak(); \
	} \
	}}
#	else
	/*
	#		define LordAssertX(x, formats, ...) { \
	static bool _ignoreAssert = false; \
	if (!_ignoreAssert && !(x)) \
	{ \
	String comment = StringUtil::ConvMBS(StringUtil::Format(formats, ##__VA_ARGS__)); \
	String msg; \
	if(!comment.empty()){ msg = "Assert comment:" + comment + "\n"; } \
	fprintf (stderr, "%s", msg.c_str()); \
	(__builtin_expect(!(x), 0) ? __assert_rtn(__func__, __FILE__, __LINE__, #x) : (void)0); \
	exit(-1);\
	}}
	*/

#		define LordAssertX(x, formats, ...) { \
	if (!(x)) \
	{ \
	String comment = StringUtil::Format(formats, ##__VA_ARGS__); \
	String msg; \
	if(!comment.empty()){ msg = "Assert comment:" + comment + "\n"; } \
	fprintf (stderr, "%s", msg.c_str()); \
	assert(x); \
	exit(-1);\
	}}

#	endif
#else
#		define LordAssertX(x, formats, ...)
#endif

#if (LORD_MODE == LORD_MODE_DEBUG)
#	define LordAssert(x)		LordAssertX(x, "")
#else
#	define LordAssert(x)
#endif
}

#endif
