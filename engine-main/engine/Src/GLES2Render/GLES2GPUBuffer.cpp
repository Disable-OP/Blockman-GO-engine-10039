#include "GLES2RenderStd.h"
#include "GLES2GPUBuffer.h"
#include "GLES2Mapping.h"
#include "GLES2Loader.h"
#include "GLES2Renderer.h"
#include "Object/Exception.h"
#include "Object/Root.h"

namespace LORD
{
	GLES2GPUBuffer::GLES2GPUBuffer(GPUBufferType type, Dword usage, const Buffer& buff)
		: GPUBuffer(type, usage, buff)
	{
		switch(type)
		{
			case GBT_VERTEX:		m_target = GL_ARRAY_BUFFER;			break;
			case GBT_INDEX:			m_target = GL_ELEMENT_ARRAY_BUFFER;	break;
			default:
			{
				LordException("Unknown GPUBufferType.");
			}
		}

		// Generate an ID for the buffer.
		glGenBuffers(1, &m_hVBO);

		// Bind the vertex buffer and load the vertex data into the vertex buffer.
		glBindBuffer(m_target, m_hVBO);

		GLenum glUsage = GLES2Mapping::MapGPUBufferUsage(usage);
		glBufferData(m_target, buff.getSize(), buff.getData(), glUsage);

		glBindBuffer(m_target, 0);


		if( Root::Instance()->getEnableFrameProfile() )
		{
			Root::Instance()->frameState().incrUploadGeometricSize( buff.getSize() );
		}

	}

	GLES2GPUBuffer::~GLES2GPUBuffer()
	{
		glDeleteBuffers(1, &m_hVBO);

		if( Root::Instance()->getEnableFrameProfile() )
		{
			Root::Instance()->frameState().decrUploadGeometricSize( m_size );
		}

	}

#ifdef LORD_LOAD_GLES_EXT
	void* GLES2GPUBuffer::map(GPUBufferAccess access)
	{
		void* pData = NULL;
		switch(access)
		{
			case GPUBuffer::GBA_WRITE:
			{
				glBindBuffer(m_target, m_hVBO);

	#if defined(LORD_LOAD_GLES_EXT)
				pData = GLES2Loader::Instance()->mapBufferOES(m_target, GL_WRITE_ONLY_OES);
	#else
				pData = glMapBufferOES(m_target, GL_WRITE_ONLY_OES);
	#endif
				glBindBuffer(m_target, 0);
				
				if( Root::Instance()->getEnableFrameProfile() )
				{
					Root::Instance()->frameState().incrLockTimes(1);
				}

			} break;
			default:
			{
				LordLogError("GLES2 only support write buffer access [GBA_WRITE].");
			}
		}

		return pData;
	}
	
	void GLES2GPUBuffer::unmap()
	{
		glBindBuffer(m_target, m_hVBO);
	
	#if defined(LORD_LOAD_GLES_EXT)
		GLES2Loader::Instance()->unmapBufferOES(m_target);
	#else
		glUnmapBufferOES(m_target);
	#endif
	
		glBindBuffer(m_target, 0);
	}
#endif
	
	bool GLES2GPUBuffer::updateSubData(uint offset, const Buffer& buff)
	{
		glBindBuffer(m_target, m_hVBO);
		GLenum glUsage = GLES2Mapping::MapGPUBufferUsage(m_usage);
		glBufferData(m_target, buff.getSize(), buff.getData(), glUsage);
		glBindBuffer(m_target, 0);

		if( Root::Instance()->getEnableFrameProfile() )
		{
			Root::Instance()->frameState().incrLockTimes(1);
		}

		return true;
	}
	
	uint GLES2GPUBuffer::getVBOHandle() const
	{
		return m_hVBO;
	}

}
