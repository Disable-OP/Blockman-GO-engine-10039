/********************************************************************
filename: 	NibbleArray.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __NIBBLE_ARRAY_HEADER__
#define __NIBBLE_ARRAY_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class NibbleArray : public ObjectAlloc
{
public:
	using value_type = i8;
	using size_type = size_t;

	NibbleArray(int size, int depthBits);
	NibbleArray(i8* arrayOfByte, int size, int depthBits);
	~NibbleArray();

	int bytes() const { return m_length; }

	// Returns the nibble of data corresponding to the passed in x, y, z. y is at most 6 bits, z is at most 4.
	int get(int x, int y, int z);
	int get(const Vector3i& pos);

	// Arguments are x, y, z, val. Sets the nibble of data at x << 11 | z << 7 | y to val.
	void set(int x, int y, int z, int val);
	void set(const Vector3i& pos, int val);

	void clear()
	{
		memset(m_data, 0, sizeof(i8)*(m_length));
	}

	void setAll(i8 v)
	{
		memset(m_data, v, sizeof(i8)*(m_length));
	}

	i8 &operator[](const size_t index)
	{
		return m_data[index];
	}

	inline size_type size() const
	{
		return m_length;
	}

	inline i8* begin()
	{
		return m_data;
	}

	inline i8* end()
	{
		return m_data + size();
	}

	inline const i8* begin() const
	{
		return m_data;
	}

	inline const i8* end() const
	{
		return m_data + size();
	}

	inline size_type max_size() const
	{
		return size();
	}

	void resize(size_type size)
	{
		throw std::logic_error("not implemented");
	}

	// Byte array of data stored in this holder. Possibly a light map or some chunk data. Data is accessed in 4-bit pieces.
	i8* m_data = nullptr;

	int m_length = 0;

	// Log base 2 of the chunk height (128); applied as a shift on Z coordinate
	int m_depthBits = 0;

	// Log base 2 of the chunk height (128) * width (16); applied as a shift on X coordinate
	int m_depthBitsPlusFour = 0;
};

}

#endif
