#include "Core.h"
#include "Exception.h"

namespace LORD
{
	Exception::Exception()
		: m_lineNum(0)
	{
	}

	Exception::Exception(const String& msg)
		: m_msg(msg)
		, m_lineNum(0)
	{
	}

	Exception::Exception(const String &msg, const String &filename, uint lineNum)
		: m_msg(msg)
		, m_filename(filename)
		, m_lineNum(0)
	{
	}

	Exception::~Exception()
	{
	}

	const String& Exception::getMessage() const
	{
		return m_msg;
	}

	void Exception::setMessage(const String& msg)
	{
		m_msg = msg;
	}

	const String& Exception::getFilename() const
	{
		return m_filename;
	}

	uint Exception::getLineNum() const
	{
		return m_lineNum;
	}
}