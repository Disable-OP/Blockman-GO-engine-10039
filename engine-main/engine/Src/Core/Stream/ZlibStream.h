/********************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Matei David, Ontario Institute for Cancer Research
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 ********************************************************************************/

 //---------------------------------------------------------
 // Copyright 2015 Ontario Institute for Cancer Research
 // Written by Matei David (matei@cs.toronto.edu)
 //---------------------------------------------------------

/********************************************************************************

	Modified from https://github.com/mateidavid/zstr by zongsheng-zhao

*********************************************************************************/

#ifndef __ZLIB_STREAM_HEADER__
#define __ZLIB_STREAM_HEADER__

#include <zlib.h>
#include "Core.h"

namespace LORD
{
	namespace ZLIB_STREAM_DETAIL
	{
		class z_stream_wrapper : public z_stream
		{
		public:
			z_stream_wrapper(bool _is_input = true, int _level = Z_DEFAULT_COMPRESSION);
			~z_stream_wrapper();
		private:
			bool is_input;
		};
	}

	class LORD_CORE_API ZlibInputStream
	{
	public:
		using int_type = int;
		using char_type = char;

	private:
		static constexpr size_t BUFFER_SIZE = 1000000;
		bool isEnd = false;
		char_type* m_input;
		char_type* m_buffer;
		char_type* m_nextRead;
		char_type* m_bufferEnd;
		ZLIB_STREAM_DETAIL::z_stream_wrapper * m_zStream;
		int_type m_readCount;

		int_type readToBuffer();

		bool bufferEmpty()
		{
			return m_bufferEnd == m_nextRead;
		}

	public:
		static constexpr int_type eof = -1;
		static constexpr int_type error = -2;

		ZlibInputStream(const ZlibInputStream&) = delete;
		ZlibInputStream(ZlibInputStream&&) = default;
		ZlibInputStream& operator=(const ZlibInputStream&) = delete;
		ZlibInputStream& operator=(ZlibInputStream&&) = default;
		ZlibInputStream(const char_type* source, size_t size);
		ZlibInputStream(const vector<char_type>::type& source);
		~ZlibInputStream();

		ZlibInputStream& read(char_type* buf, int_type count);
		ZlibInputStream& get(char_type& c);
		explicit operator bool()
		{
			return m_readCount >= 0;
		}
		size_t gcount() 
		{
			return m_readCount >= 0 ? m_readCount : 0;
		}
	};

	class LORD_CORE_API ZlibOutputStream
	{
	public:
		using int_type = int;
		using char_type = char;

	private:
		static constexpr size_t BUFFER_SIZE = 1000000;
		char_type* m_inputBuffer;
		char_type* m_outputBuffer;
		char_type* m_nextWrite;
		ZLIB_STREAM_DETAIL::z_stream_wrapper * m_zStream;
		vector<char_type>::type * m_dest;

	public:
		static constexpr int_type error = -2;

		ZlibOutputStream(const ZlibOutputStream&) = delete;
		ZlibOutputStream(ZlibOutputStream&&) = default;
		ZlibOutputStream& operator=(const ZlibOutputStream&) = delete;
		ZlibOutputStream& operator=(ZlibOutputStream&&) = default;
		ZlibOutputStream(vector<char_type>::type& dest);
		~ZlibOutputStream();

		int_type deflateLoop(int flushFlag);

		int_type write(const char_type* buf, int_type count);
		int_type put(const char_type& c);
		int_type flush();
	};
}

#endif // !__ZLIB_STREAM_HEADER__

