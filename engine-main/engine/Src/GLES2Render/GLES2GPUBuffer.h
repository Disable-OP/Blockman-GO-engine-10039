/********************************************************************
filename: 	GLES2GPUBuffer.h
file path:	dev\engine\Src\GLES2Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GLES2GPUBUFFER_H__
#define __LORD_GLES2GPUBUFFER_H__

#include "Render/GPUBuffer.h"

namespace LORD
{
	class LORD_GLES2RENDER_API GLES2GPUBuffer: public GPUBuffer
	{
	public:
		GLES2GPUBuffer(GPUBufferType type, Dword usage, const Buffer& buff);
		~GLES2GPUBuffer();

	public:
#ifdef LORD_LOAD_GLES_EXT
		void*			map(GPUBufferAccess access);
		void			unmap();
#else
		void*			map(GPUBufferAccess access) { return NULL; }
		void			unmap(){}
#endif // LORD_LOAD_GLES_EXT

		bool			updateSubData(uint offset, const Buffer& buff);
		uint			getVBOHandle() const;

	private:
		GLenum			m_target;
		uint			m_hVBO;
	};
}

#endif
