#include "ZlibStream.h"
#include <algorithm>
#include "Core.h"

namespace LORD
{
	namespace ZLIB_STREAM_DETAIL
	{
		z_stream_wrapper::z_stream_wrapper(bool _is_input, int _level)
			: is_input(_is_input)
		{
			this->zalloc = Z_NULL;
			this->zfree = Z_NULL;
			this->opaque = Z_NULL;
			int ret;
			if (is_input)
			{
				this->avail_in = 0;
				this->next_in = Z_NULL;
				ret = inflateInit2(this, 15 + 32);
			}
			else
			{
				ret = deflateInit2(this, _level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
			}
			if (ret != Z_OK)
			{
				throw std::runtime_error(("failed to initialize zlib. code: " + StringUtil::ToString(ret)).c_str());
			}
		}

		z_stream_wrapper::~z_stream_wrapper()
		{
			if (is_input)
			{
				inflateEnd(this);
			}
			else
			{
				deflateEnd(this);
			}
		}
	}


	ZlibInputStream::int_type ZlibInputStream::readToBuffer()
	{
		if (isEnd)
		{
			return eof;
		}

		int ret;
		m_nextRead = m_buffer;
		do {
			m_zStream->next_out = reinterpret_cast<decltype(m_zStream->next_out)>(m_buffer);
			m_zStream->avail_out = BUFFER_SIZE;
			ret = inflate(m_zStream, Z_NO_FLUSH);
			m_bufferEnd = reinterpret_cast<decltype(m_bufferEnd)>(m_zStream->next_out);
		} while (bufferEmpty() && ret == Z_OK);

		if (ret == Z_OK)
		{
			return 0;
		}

		if (ret == Z_STREAM_END)
		{
			isEnd = true;
			return 0;
		}

		return error;
	}

	ZlibInputStream::ZlibInputStream(const char_type * source, size_t size)
	{
		m_input = new char_type[size];
		std::copy_n(source, size, m_input);
		m_bufferEnd = m_nextRead = m_buffer = new char_type[BUFFER_SIZE];
		m_zStream = new ZLIB_STREAM_DETAIL::z_stream_wrapper(true);
		m_zStream->next_in = reinterpret_cast<decltype(m_zStream->next_in)>(m_input);
		m_zStream->avail_in = size;
	}

	ZlibInputStream::ZlibInputStream(const vector<char_type>::type & source)
		: ZlibInputStream(source.data(), source.size())
	{
	}

	ZlibInputStream::~ZlibInputStream()
	{
		delete[] m_buffer;
		delete[] m_input;
		delete m_zStream;
	}

	ZlibInputStream& ZlibInputStream::read(char_type * buf, int_type count)
	{
		if (count <= 0)
		{
			return *this;
		}
		int_type readLeft = count;
		do
		{
			int_type ret;
			if (bufferEmpty())
			{
				ret = readToBuffer();
				if (ret == eof)
				{
					break;
				}
				else if (ret != 0)
				{
					m_readCount = ret;
					return *this;
				}
			}

			using std::min;
			ret = min(static_cast<int_type>(m_bufferEnd - m_nextRead), readLeft);
			std::copy_n(m_nextRead, ret, buf);
			m_nextRead += ret;
			readLeft -= ret;
			buf += ret;
		} while (readLeft > 0);

		if (readLeft == count)
		{
			m_readCount = eof;
		}
		else
		{
			m_readCount = count - readLeft;
		}
		return *this;
	}

	ZlibInputStream& ZlibInputStream::get(char_type & c)
	{
		int_type ret;
		if (bufferEmpty())
		{
			ret = readToBuffer();
			if (ret != 0)
			{
				m_readCount = ret;
				return *this;
			}
		}

		c = *m_nextRead++;
		m_readCount = 1;
		return *this;
	}

	ZlibOutputStream::ZlibOutputStream(vector<char_type>::type & dest) : m_dest(&dest)
	{
		m_outputBuffer = new char_type[BUFFER_SIZE];
		m_nextWrite = m_inputBuffer = new char_type[BUFFER_SIZE];
		m_zStream = new ZLIB_STREAM_DETAIL::z_stream_wrapper(false);
	}

	ZlibOutputStream::~ZlibOutputStream()
	{
		flush();
		delete m_zStream;
		delete[] m_inputBuffer;
		delete[] m_outputBuffer;
	}

	ZlibOutputStream::int_type ZlibOutputStream::deflateLoop(int flushFlag)
	{
		m_zStream->next_in = reinterpret_cast< decltype(m_zStream->next_in) >(m_inputBuffer);
		m_zStream->avail_in = m_nextWrite - m_inputBuffer;
		int ret;
		do
		{
			m_zStream->next_out = reinterpret_cast< decltype(m_zStream->next_out) >(m_outputBuffer);
			m_zStream->avail_out = BUFFER_SIZE;
			ret = deflate(m_zStream, flushFlag);
			if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR)
			{
				return error;
			}
			auto bufferEnd = reinterpret_cast<decltype(m_outputBuffer)>(m_zStream->next_out);
			m_dest->reserve(m_dest->size() + (bufferEnd - m_outputBuffer));
			for (auto p = m_outputBuffer; p < bufferEnd; ++p)
			{
				m_dest->push_back(*p);
			}
		} while (ret == Z_OK || (ret == Z_BUF_ERROR && m_zStream->avail_out == 0));
		m_nextWrite = m_inputBuffer;
		return 0;
	}

	ZlibOutputStream::int_type ZlibOutputStream::write(const char_type * buf, int_type count)
	{
		if (m_inputBuffer + BUFFER_SIZE < m_nextWrite + count)
		{
			auto ret = deflateLoop(Z_NO_FLUSH);
			if (ret != 0)
			{
				return ret;
			}
		}
		std::copy_n(buf, count, m_nextWrite);
		m_nextWrite += count;
		return 0;
	}

	ZlibOutputStream::int_type ZlibOutputStream::put(const char_type & c)
	{
		if (m_inputBuffer + BUFFER_SIZE < m_nextWrite + 1)
		{
			auto ret = deflateLoop(Z_NO_FLUSH);
			if (ret != 0)
			{
				return ret;
			}
		}
		*m_nextWrite++ = c;
		return 0;
	}

	ZlibOutputStream::int_type ZlibOutputStream::flush()
	{
		auto ret = deflateLoop(Z_NO_FLUSH);
		if (ret != 0)
		{
			return ret;
		}
		m_zStream->next_in = nullptr;
		m_zStream->avail_in = 0;
		ret = deflateLoop(Z_FINISH);
		if (ret != 0)
		{
			return ret;
		}
		deflateReset(m_zStream);
		return 0;
	}
}