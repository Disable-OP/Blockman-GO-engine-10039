#ifndef __IO_HEADER__
#define __IO_HEADER__

#include <stdexcept>
#include <istream>
#include <ostream>

namespace BLOCKMAN
{
	class StreamError : public std::runtime_error
	{
		using std::runtime_error::runtime_error;
	};

	template<typename T, typename OutputStream>
	void writeBinary(const T &value, OutputStream &os)
	{
		for (int i = sizeof(T) - 1; i >= 0; --i)
		{
			os.put(static_cast<uint8_t>(value >> i * 8));
		}
	}

	template<typename OutputStream>
	inline void writeBinary(const bool &value, OutputStream &os)
	{
		writeBinary(value ? '\1' : '\0', os);
	}

	template<typename OutputStream>
	inline void writeBinary(const float &value, OutputStream &os)
	{
		writeBinary(reinterpret_cast<const uint32_t&>(value), os);
	}

	template<typename OutputStream>
	inline void writeBinary(const double &value, OutputStream &os)
	{
		writeBinary(reinterpret_cast<const uint64_t&>(value), os);
	}

	// Don't touch this function!! This function is called very frequently and 
	// is the bottle neck of map loading performance. Any changes might cause 
	// huge performance decrease
	// TODO: return false for error
	template<typename T, typename InputStream>
	void readBinary(T &value, InputStream &is)
	{
		char c;
		if (!is.get(c))
		{
			throw StreamError("IO::readBinary1::failed to read input stream");
		}
		value = static_cast<unsigned char>(c);
		for (int i = 1; i < sizeof(T); ++i)
		{
			if (!is.get(c))
			{
				throw StreamError("IO::readBinary2::failed to read input stream");
			}
			value <<= 8;
			value |= static_cast<unsigned char>(c);
		}
	}

	template<typename InputStream>
	inline void readBinary(float &value, InputStream &is)
	{
		readBinary(reinterpret_cast<uint32_t&>(value), is);
	}

	template<typename InputStream>
	inline void readBinary(double &value, InputStream &is)
	{
		readBinary(reinterpret_cast<uint64_t&>(value), is);
	}
}

#endif // !__IO_HEADER__

