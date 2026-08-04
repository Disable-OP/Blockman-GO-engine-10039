/********************************************************************
filename: 	GLES2RenderStd.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2RENDERSTD_H__
#define __LORD_GLES2RENDERSTD_H__

#include "../Core/Core.h"

#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#	ifdef LORD_GLES2RENDER_EXPORTS
#		define LORD_GLES2RENDER_API __declspec(dllexport)
#	else
#		define LORD_GLES2RENDER_API __declspec(dllimport)
#	endif
#else
#	define LORD_GLES2RENDER_API
#endif

#ifndef GL_GLEXT_PROTOTYPES
#  define  GL_GLEXT_PROTOTYPES
#endif

#if (LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
#	include <OpenGLES/ES2/gl.h>
#	include <OpenGLES/ES2/glext.h>
#   ifdef __OBJC__
#       include <OpenGLES/EAGL.h>
#   endif
#elif (LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_NACL)
#	include <GLES2/gl2platform.h>
#	include <GLES2/gl2.h>
#	include <GLES2/gl2ext.h>
#	include <EGL/egl.h>
#	if (LORD_PLATFORM == LORD_PLATFORM_NACL)
#		include "ppapi/cpp/completion_callback.h"
#       include "ppapi/cpp/instance.h"
#       include "ppapi/c/ppp_graphics_3d.h"
#       include "ppapi/cpp/graphics_3d.h"
#       include "ppapi/cpp/graphics_3d_client.h"
#		include "ppapi/gles2/gl2ext_ppapi.h"
#       undef GL_OES_get_program_binary
#       undef GL_OES_mapbuffer
#	endif
#else
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#   include <EGL/egl.h>
#endif

#if ( (LORD_PLATFORM == LORD_PLATFORM_ANDROID) || (LORD_PLATFORM == LORD_PLATFORM_WINDOWS) )
//#	define LORD_LOAD_GLES_EXT
#endif

namespace LORD
{
	enum GLES2Error
	{
		GLES2ERR_NO_ERROR,
		GLES2ERR_INVALID_ENUM,
		GLES2ERR_INVALID_VALUE,
		GLES2ERR_INVALID_OPERATION,
		GLES2ERR_STACK_OVERFLOW,
		GLES2ERR_STACK_UNDERFLOW,
		GLES2ERR_OUT_OF_MEMORY,
	};
}

#endif
