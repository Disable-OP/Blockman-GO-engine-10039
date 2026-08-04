/********************************************************************
filename: 	Exception.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_EXCEPTION_H__
#define __LORD_EXCEPTION_H__

#include "Memory/LordMemory.h"
#include "Util/StringUtil.h"

namespace LORD
{
	class LORD_CORE_API Exception: public ObjectAlloc
	{

	public:
		Exception();
		Exception(const String& msg);
		Exception(const String& msg, const String& filename, uint lineNum);
		virtual ~Exception();

		//Methods
	public:
		virtual const String&	getMessage() const;
		virtual void			setMessage(const String &msg);

		virtual const String&	getFilename() const;
		virtual uint			getLineNum() const;

	protected:
		String		m_msg;
		String		m_filename;
		int			m_lineNum;
	};

	inline void __LordThrowException(const String& msg, const char* filename, uint lineNum)
	{
		throw Exception(msg, filename, lineNum);
	};

#define LordException(formats, ...) __LordThrowException(LORD::StringUtil::Format(formats, ##__VA_ARGS__), __FILE__, __LINE__)
}

#endif