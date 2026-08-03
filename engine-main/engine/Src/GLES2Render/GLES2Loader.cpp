#include "GLES2RenderStd.h"
#include "GLES2Loader.h"

namespace LORD
{
	GLES2Loader::GLES2Loader()
	{
	}

	GLES2Loader::~GLES2Loader()
	{
	}

	bool GLES2Loader::initialize()
	{
	#if ( (LORD_PLATFORM == LORD_PLATFORM_ANDROID) || (LORD_PLATFORM == LORD_PLATFORM_WINDOWS) )
		genVertexArraysOES = (LORD_PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress("glGenVertexArraysOES");
		if(!genVertexArraysOES)
			return false;

		bindVertexArrayOES = (LORD_PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress("glBindVertexArrayOES");
		if(!bindVertexArrayOES)
			return false;

		deleteVertexArraysOES = (LORD_PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress("glDeleteVertexArraysOES");
		if(!deleteVertexArraysOES)
			return false;

		isVertexArrayOES = (LORD_PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress("glIsVertexArrayOES");
		if(!isVertexArrayOES)
			return false;

		mapBufferOES = (LORD_PFNGLMAPBUFFEROESPROC)eglGetProcAddress("glMapBufferOES");
		if(!mapBufferOES)
			return false;

		unmapBufferOES = (LORD_PFNGLUNMAPBUFFEROESPROC)eglGetProcAddress("glUnmapBufferOES");
		if(!unmapBufferOES)
			return false;
	#endif
		return true;
	}

}
