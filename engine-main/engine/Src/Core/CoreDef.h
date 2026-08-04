/********************************************************************
filename: 	CoreDef.h
file path:	dev\engine\Src\Core\Common\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_COREDEF_H__
#define __LORD_COREDEF_H__

// Lord Engine version
#define LORD_CORE_VER 1.0

// Initial compiler-related stuff to set.
#define LORD_COMPILER_MSVC		1
#define LORD_COMPILER_CLANG		2
#define LORD_COMPILER_GNUC		3

// Initial platform stuff to set.
#define LORD_PLATFORM_WINDOWS   1
#define LORD_PLATFORM_LINUX     2
#define LODD_PLATFORM_MAC_OSX   3
#define LORD_PLATFORM_MAC_IOS   4
#define LORD_PLATFORM_ANDROID   5
#define LORD_PLATFORM_NACL		6

// CPU Architecture
#define LORD_ARCH_32BIT         1
#define LORD_ARCH_64BIT         2
#define LORD_ARCH_ARM			3

// Endian
#define LORD_ENDIAN_LITTLE      1
#define LORD_ENDIAN_BIG         2

// Charset
#define LORD_CHARSET_UNICODE    1
#define LORD_CHARSET_MULTIBYTE  2

// Precision
#define LORD_PREC_FLOAT         1
#define LORD_PREC_DOUBLE        2

// Mode
#define LORD_MODE_DEBUG         1
#define LORD_MODE_RELEASE       2

// Memory Allocator
#define LORD_MEMORY_ALLOCATOR_NED           1
#define LORD_MEMORY_ALLOCATOR_NEDPOOLING    2
#define LORD_MEMORY_ALLOCATOR_HARVEYS		3
#define LORD_MEMORY_ALLOCATOR_BINNED        4
#define LORD_MEMORY_ALLOCATOR_DEFAULT       5

// STL Memory Allocator
#define LORD_STL_MEMORY_ALLOCATOR_STANDARD	1
#define LORD_STL_MEMORY_ALLOCATOR_CUSTOM	2

// Compiler type and version recognition
#if defined( _MSC_VER )
#   define LORD_COMPILER            LORD_COMPILER_MSVC
#	if _MSC_VER >= 1700
#		define LORD_COMPILER_VERSION 110
#	elif _MSC_VER >= 1600
#		define LORD_COMPILER_VERSION 100
#	elif _MSC_VER >= 1500
#		define LORD_COMPILER_VERSION 90
#	elif _MSC_VER >= 1400
#		define LORD_COMPILER_VERSION 80
#	elif _MSC_VER >= 1300
#		define LORD_COMPILER_VERSION 70
#	endif
#elif defined( __clang__ )
#   define LORD_COMPILER			LORD_COMPILER_CLANG
#   define LORD_COMPILER_VERSION	(((__clang_major__)*100) + \
(__clang_minor__*10) + \
__clang_patchlevel__)
#elif defined( __GNUC__ )
#   define LORD_COMPILER            LORD_COMPILER_GNUC
#   define LORD_COMPILER_VERSION    (((__GNUC__)*100) + \
(__GNUC_MINOR__*10) + \
__GNUC_PATCHLEVEL__)
#else
#   error "Unknown compiler. Abort! Abort!"
#endif

// Platform recognition
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#	define LORD_PLATFORM    LORD_PLATFORM_WINDOWS
#elif defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#	define LORD_PLATFORM    LORD_PLATFORM_WINDOWS
#elif defined(__APPLE_CC__)
// Device                                                     Simulator
// Both requiring OS version 4.0 or greater
#   if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000 || __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000
#       define LORD_PLATFORM        LORD_PLATFORM_MAC_IOS
#   else
#       define LORD_PLATFORM        LORD_PLATFORM_MAC_OSX
#   endif
#elif defined(__ANDROID__)
#	define LORD_PLATFORM    LORD_PLATFORM_ANDROID
#elif defined(linux) || defined(__linux) || defined(__linux__)
#	define LORD_PLATFORM	LORD_PLATFORM_LINUX
#elif defined(__native_client__)
#   define LORD_PLATFORM	LORD_PLATFORM_NACL
#else
#	error "Couldn't recognize platform"
#endif

// CPU architechture type recognition
#if defined(_M_X64) || defined(__x86_64__) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#	define LORD_ARCHITECTURE        LORD_ARCH_64BIT
#elif defined(_M_ARM) || defined(__arm__)
#	define LORD_ARCHITECTURE		LORD_ARCH_ARM
//#elif defined(_M_IX86) || defined(__i386__)
#else
#	define LORD_ARCHITECTURE        LORD_ARCH_32BIT
#endif

// Disable MSVC warning
#if (LORD_COMPILER == LORD_COMPILER_MSVC)
#	pragma warning(disable:4251 4275 4819)
#	ifndef _CRT_SECURE_NO_DEPRECATE
#		define _CRT_SECURE_NO_DEPRECATE
#	endif
#	ifndef _SCL_SECURE_NO_DEPRECATE
#		define _SCL_SECURE_NO_DEPRECATE
#	endif
#endif

// Endian Settings
static union { char c[4]; unsigned nVar; } _LordEndian = {{ 'L', '?', '?', 'B' } };
static const int _LordEndianFlag = ((char)_LordEndian.nVar == 'L'? LORD_ENDIAN_LITTLE : LORD_ENDIAN_BIG);
#define LORD_ENDIAN     _LordEndianFlag

// Charset Settings
#if defined(_UNICODE) || defined(UNICODE)
#	define LORD_CHARSET     LORD_CHARSET_UNICODE
#else
#	define LORD_CHARSET     LORD_CHARSET_MULTIBYTE
#endif

// Asserts expression is true at compile-time
#define LORD_COMPILER_ASSERT(x)	typedef int COMPILER_ASSERT_[!!(x)]

// Precision define
#if defined(DOUBLE_PRECISION)
#	define LORD_PRECISION    LORD_PREC_DOUBLE
#else
#	define LORD_PRECISION      LORD_PREC_FLOAT
#endif

// mode
#if defined(_DEBUG) || defined(DEBUG) || !defined(NDEBUG)
#	define LORD_MODE        LORD_MODE_DEBUG
#else
#	define LORD_MODE        LORD_MODE_RELEASE
#endif

#define LORD_UNUSED(a) (void)a

#define LORD_ALIGN16 __declspec(align(16))
#define LORD_SIMD_ALIGNMENT 16

// Lord Engine Memory Managed
#if (LORD_MODE == LORD_MODE_DEBUG && LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#define LORD_MEMORY_TRACKER  //如果你想查内存泄露，就把注释去掉。
#endif

// Lord memory allocator
#if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
#   define LORD_MEMORY_ALLOCATOR		LORD_MEMORY_ALLOCATOR_NEDPOOLING
#else
#   define LORD_MEMORY_ALLOCATOR		LORD_MEMORY_ALLOCATOR_BINNED
#endif

//#define LORD_MEMORY_ALLOCATOR		LORD_MEMORY_ALLOCATOR_NED
//#define LORD_MEMORY_ALLOCATOR       LORD_MEMORY_ALLOCATOR_DEFAULT

// Lord STL memory allocator
#define LORD_STL_MEMORY_ALLOCATOR	LORD_STL_MEMORY_ALLOCATOR_CUSTOM
// Log
#define LORD_LOG_FILENAME			"Lord.log"
#define LORD_MEMORY_LEAKS_FILENAME	"MemoryLeaks.log"

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS && !LORD_STATIC)
#	ifdef LORD_CORE_EXPORTS
#		define LORD_CORE_API __declspec(dllexport)
#	else
#		define LORD_CORE_API __declspec(dllimport)
#	endif
#else
#	define LORD_CORE_API
#endif

#if (LORD_COMPILER == LORD_COMPILER_MSVC)
#	define _CRT_NON_CONFORMING_SWPRINTFS	
#endif

//#define LORD_USE_RENDERTARGET

namespace LORD
{
	enum PixelFormat
	{
		// Unknown pixel format.
		PF_UNKNOWN = 0,	

		//////////////////////////////////////////////////////////////////////////
		// 8-bit per channel pixel format

		// 8-bit pixel format, all bits red.
		PF_R8_UNORM, 
		PF_R8_SNORM, 
		PF_R8_UINT, 
		PF_R8_SINT,

		// 8-bit pixel format, all bits alpha
		PF_A8_UNORM,

		// 16-bit, 2-channel floating point pixel format, 8-bit red, 8-bit green
		PF_RG8_UNORM, 
		PF_RG8_SNORM, 
		PF_RG8_UINT, 
		PF_RG8_SINT, 

		// 24-bit pixel format, 8 bits for red, green and blue.
		PF_RGB8_UNORM, 
		PF_RGB8_SNORM, 
		PF_RGB8_UINT, 
		PF_RGB8_SINT, 

		// 24-bit pixel format, 8 bits for blue, green and red.
		PF_BGR8_UNORM, 

		// 32-bit pixel format, 8 bits for red, green, blue and alpha.
		PF_RGBA8_UNORM, 
		PF_RGBA8_SNORM, 
		PF_RGBA8_UINT, 
		PF_RGBA8_SINT, 

		// 32-bit pixel format, 8 bits for blue, green and red.
		PF_BGRA8_UNORM, 

		//////////////////////////////////////////////////////////////////////////
		// 16-bit per channel pixel format

		// 16-bit pixel format, all bits red.
		PF_R16_UNORM, 
		PF_R16_SNORM, 
		PF_R16_UINT, 
		PF_R16_SINT, 
		PF_R16_FLOAT, 

		// 32-bit, 2-channel floating point pixel format, 16-bit red, 16-bit green
		PF_RG16_UNORM, 
		PF_RG16_SNORM, 
		PF_RG16_UINT, 
		PF_RG16_SINT, 
		PF_RG16_FLOAT, 

		// 48-bit pixel format, 16 bits for red, green and blue.
		PF_RGB16_UNORM, 
		PF_RGB16_SNORM, 
		PF_RGB16_UINT, 
		PF_RGB16_SINT, 
		PF_RGB16_FLOAT, 

		// 64-bit pixel format, 16 bits for red, green, blue and alpha.
		PF_RGBA16_UNORM, 
		PF_RGBA16_SNORM, 
		PF_RGBA16_UINT, 
		PF_RGBA16_SINT, 
		PF_RGBA16_FLOAT, 

		//////////////////////////////////////////////////////////////////////////
		// 32-bit per channel pixel format

		// 32-bit pixel format, all bits red.
		PF_R32_UNORM, 
		PF_R32_SNORM, 
		PF_R32_UINT, 
		PF_R32_SINT, 
		PF_R32_FLOAT, 

		// 64-bit, 2-channel floating point pixel format, 32-bit red, 32-bit green
		PF_RG32_UNORM, 			// Unsupported Serialization
		PF_RG32_SNORM, 			// Unsupported Serialization
		PF_RG32_UINT, 			// Unsupported Serialization
		PF_RG32_SINT, 			// Unsupported Serialization
		PF_RG32_FLOAT, 

		// 96-bit pixel format, 32 bits for red, green and blue.
		PF_RGB32_UNORM,			// Unsupported Serialization
		PF_RGB32_SNORM,			// Unsupported Serialization
		PF_RGB32_UINT,			// Unsupported Serialization
		PF_RGB32_SINT,			// Unsupported Serialization
		PF_RGB32_FLOAT, 

		// 128-bit pixel format, 32 bits for red, green, blue and alpha.
		PF_RGBA32_UNORM, 		// Unsupported Serialization
		PF_RGBA32_SNORM, 		// Unsupported Serialization
		PF_RGBA32_UINT, 		// Unsupported Serialization
		PF_RGBA32_SINT, 		// Unsupported Serialization
		PF_RGBA32_FLOAT, 

		//////////////////////////////////////////////////////////////////////////
		// special pixel format

		// Depth-stencil format.
		PF_D16_UNORM, 
		PF_D24_UNORM_S8_UINT, 
		PF_D32_FLOAT, 

		//////////////////////////////////////////////////////////////////////////
		// block-compression pixel format

		// Four-component block-compression format, 1/8 original size, DDS (DirectDraw Surface) DXT1 format.
		PF_BC1_UNORM, 
		PF_BC1_UNORM_SRGB, 
		// Four-component block-compression format, 1/4 original size, DDS (DirectDraw Surface) DXT3 format.
		PF_BC2_UNORM, 
		PF_BC2_UNORM_SRGB, 
		// Four-component block-compression format, 1/4 original size, DDS (DirectDraw Surface) DXT5 format.
		PF_BC3_UNORM, 
		PF_BC3_UNORM_SRGB, 
		// One-component block-compression format, 1/4 original size.
		PF_BC4_UNORM, 
		PF_BC4_SNORM, 
		// Two-component block-compression format, 1/2 original size.
		PF_BC5_UNORM, 
		PF_BC5_SNORM, 

		//////////////////////////////////////////////////////////////////////////

		PF_COUNT, 
	};

	/**
	* Flags defining some on/off properties of pixel formats
	*/
	enum PixelFormatFlags
	{
		// Format is need to normalize
		PFF_NORMALIZED		= 0x00000001,
		// This format has an alpha channel
		PFF_ALPHA			= 0x00000002,
		// This format is compressed. This invalidates the values in elemBytes,
		// elemBits and the bit counts as these might not be fixed in a compressed format.
		PFF_COMPRESSED		= 0x00000004,
		// This is a floating point format
		PFF_FLOAT			= 0x00000008,
		// This is a depth format (for depth textures)
		PFF_DEPTH			= 0x00000010,
		// Format is in native endian. Generally true for the 16, 24 and 32 bits
		// formats which can be represented as machine integers.
		PFF_NATIVEENDIAN	= 0x00000020,
		// This is an intensity format instead of a RGB one. The luminance
		// replaces R,G and B. (but not A)
		PFF_LUMINANCE		= 0x00000040,
		// This is stencil format
		PFF_STENCIL			= 0x00000080
	};

	/** Pixel channel type*/
	enum PixelChannelFormat
	{
		PCF_NONE, 
		PCF_R, 
		PCF_G, 
		PCF_B, 
		PCF_A, 
		PCF_D, 
		PCF_S, 
		PCF_BC, 
		PCF_E, 
	};

	/** Pixel channel type*/
	enum PixelChannelType
	{
		PCT_NONE,
		PCT_UNORM,
		PCT_SNORM,
		PCT_SINT,
		PCT_UINT,
		PCT_FLOAT,
		PCT_TYPELESS,
		PCT_UNORM_SRGB,
		PCT_SHAREDEXP, 
	};

	enum ImageFormat
	{
		IF_UNKNOWN, 
		IF_BMP,					//!< Microsoft Windows Bitmap - .bmp extension
		IF_DDS,					//!< DirectDraw Surface - .dds extension
		IF_JPG,					//!< JPEG - .jpg, .jpe and .jpeg extensions
		IF_PNG,					//!< Portable Network Graphics - .png extension
		IF_PVR,					//!< PowerVR format - .pvr extension
		IF_TGA,					//!< TrueVision Targa File - .tga, .vda, .icb and .vst extensions
	};
	
	const static unsigned int TEXTURE_UNIT0 = 0;
	const static unsigned int TEXTURE_UNIT1 = 1;
	const static unsigned int TEXTURE_UNIT2 = 2;
}

#include "Scene/SceneDefine.h"

#endif
