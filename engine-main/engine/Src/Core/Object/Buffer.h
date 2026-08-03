/********************************************************************
filename: 	Buffer.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_BUFFER_H__
#define __LORD_BUFFER_H__

#include "Core.h"

namespace LORD
{
	class LORD_CORE_API Buffer: public ObjectAlloc
	{
	public:
		Buffer();
		Buffer(uint size, void* pData = NULL, bool bAutoFree = false);
		virtual ~Buffer();

	public:
		virtual void			allocate(uint size);
		virtual void			clear();
		virtual void			set(uint size, void *pData);
		virtual void			copyBuffer(const Buffer &buff);
		virtual Byte*			getData() const;
		virtual uint			getSize() const;
		virtual uint			takeData(Byte* &pData);

	protected:
		Byte*		m_pData;
		uint		m_size;
		bool		m_bAutoFree;
	};
}


#endif