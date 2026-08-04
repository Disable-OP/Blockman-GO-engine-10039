#include "Core.h"
#include "Buffer.h"
#include "Util/AssertX.h"

namespace LORD
{

	Buffer::Buffer()
		: m_pData(NULL)
		, m_size(0)
		, m_bAutoFree(false)
	{
	}

	Buffer::Buffer(uint size, void *pData, bool bAutoFree)
		: m_bAutoFree(bAutoFree)
	{
		set(size, pData);
	}

	Buffer::~Buffer()
	{
		clear();
	}

	void Buffer::allocate(uint size)
	{
		LordAssertX(size > 0, "Buffer allocated 0 size.");

		clear();

		m_pData = (Byte*)LordMalloc(sizeof(Byte)*size);
		m_size = size;
		m_bAutoFree = true;
	}

	void Buffer::clear()
	{
		if (m_bAutoFree && m_pData)
		{
			LordFree(m_pData);
		}

		m_pData = NULL;
		m_size = 0;
		m_bAutoFree = false;
	}

	void Buffer::set(uint size, void* pData)
	{
		clear();

		m_pData = (Byte*)pData;
		m_size = size;
		m_bAutoFree = false;
	}

	void Buffer::copyBuffer(const Buffer& buff)
	{
		allocate(buff.getSize());
		memcpy(m_pData, buff.getData(), buff.getSize());
	}

	Byte* Buffer::getData() const
	{
		return m_pData;
	}

	uint Buffer::getSize() const
	{ 
		return m_size; 
	}

	uint Buffer::takeData(Byte*& pData)
	{
		pData = m_pData;
		m_pData = NULL;
		uint size = m_size;
		m_size = 0;
		m_bAutoFree = false;
		return size;
	}
}