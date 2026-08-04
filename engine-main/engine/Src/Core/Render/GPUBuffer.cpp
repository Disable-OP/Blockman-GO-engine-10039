#include "Core.h"
#include "GPUBuffer.h"

namespace LORD
{
	GPUBuffer::GPUBuffer(GPUBufferType type, Dword usage, const Buffer& buff)
		: m_type(type)
		, m_usage(usage)
		, m_size(buff.getSize())
	{
	}

	GPUBuffer::~GPUBuffer()
	{
	}

	uint GPUBuffer::getSize() const
	{
		return m_size;
	}

}