/********************************************************************
filename: 	GPUBuffer.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_GPUBUFFER_H__
#define __LORD_GPUBUFFER_H__

#include "Core.h"
#include "Object/Buffer.h"

namespace LORD
{
	class LORD_CORE_API GPUBuffer: public ObjectAlloc
	{	
	public:
		enum GPUBufferType
		{
			GBT_VERTEX,
			GBT_INDEX,
		};

		enum GPUBufferUsage
		{
			GBU_CPU_READ		= 0x00000001,
			GBU_CPU_WRITE		= 0x00000002,
			// texture is used as shader resource
			GBU_GPU_READ		= 0x00000004,
			// texture is used as depth or render target (depend on pixel format)
			GBU_GPU_WRITE		= 0x00000008,
			GBU_GPU_UNORDERED	= 0x00000010,

			GBU_DEFAULT			= GBU_GPU_READ | GBU_GPU_WRITE,
			GBU_STATIC			= GBU_GPU_READ,
			GBU_DYNAMIC			= GBU_GPU_READ | GBU_CPU_WRITE,
		};

		enum GPUBufferAccess
		{
			GBA_READ,
			GBA_WRITE,
			GBA_READ_WRITE,
		};

	public:
		GPUBuffer(GPUBufferType type, Dword usage, const Buffer& buff);
		virtual ~GPUBuffer();

		friend class GPUBufferManager;

	public:
		virtual void*		map(GPUBufferAccess access) = 0;
		virtual void		unmap() = 0;
		virtual bool		updateSubData(uint offset, const Buffer& buff) = 0;
		virtual uint		getSize() const;

	protected:
		GPUBufferType		m_type;
		Dword				m_usage;
		uint				m_size;
	};
}

#endif