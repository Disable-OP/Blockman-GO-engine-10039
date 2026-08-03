#include "NibbleArray.h"

namespace BLOCKMAN
{

NibbleArray::NibbleArray(int size, int depthBits)
{
	m_data = (i8*)LordMalloc(sizeof(i8) * (size>>1)); 
	memset(m_data, 0, sizeof(i8)*(size >> 1));
	m_length = size >> 1;
	m_depthBits = depthBits;
	m_depthBitsPlusFour = depthBits + 4;
}

NibbleArray::NibbleArray(i8* arrayOfByte, int size, int depthBits)
{
	m_data = arrayOfByte;
	m_length = size;
	m_depthBits = depthBits;
	m_depthBitsPlusFour = depthBits + 4;
}

NibbleArray::~NibbleArray()
{
	LordSafeFree(m_data);
}

int NibbleArray::get(int x, int y, int z)
{
	int logicIdx = y << m_depthBitsPlusFour | z << m_depthBits | x;
	int physIdx = logicIdx >> 1;
	int bLow = logicIdx & 1;
	return bLow == 0 ? m_data[physIdx] & 15 : m_data[physIdx] >> 4 & 15;
}

int NibbleArray::get(const Vector3i& pos)
{
	int idx = pos.y << m_depthBitsPlusFour | pos.z << m_depthBits | pos.x;
	int physIdx = idx >> 1;
	int bLow = idx & 1;
	return bLow == 0 ? m_data[physIdx] & 15 : m_data[physIdx] >> 4 & 15;
}

void NibbleArray::set(int x, int y, int z, int val)
{
	int logicIdx = y << m_depthBitsPlusFour | z << m_depthBits | x;
	int physIdx = logicIdx >> 1;
	int bLow = logicIdx & 1;

	if (bLow == 0)
	{
		m_data[physIdx] = (i8)(m_data[physIdx] & 240 | val & 15);
	}
	else
	{
		m_data[physIdx] = (i8)(m_data[physIdx] & 15 | (val & 15) << 4);
	}
}

void NibbleArray::set(const Vector3i& pos, int val)
{
	int logicIdx = pos.y << m_depthBitsPlusFour | pos.z << m_depthBits | pos.x;
	int physIdx = logicIdx >> 1;
	int bLow = logicIdx & 1;

	if (bLow == 0)
	{
		m_data[physIdx] = (i8)(m_data[physIdx] & 240 | val & 15);
	}
	else
	{
		m_data[physIdx] = (i8)(m_data[physIdx] & 15 | (val & 15) << 4);
	}
}

}