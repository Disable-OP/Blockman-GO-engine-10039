/********************************************************************
filename: 	Bitset.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-23
*********************************************************************/
#ifndef __BITSET_HEADER__
#define __BITSET_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{


#define div8(n) ((n & 0xFFFFFFF8) >> 3)					//	div8(n) = n / 8;
#define mod8(n) (n & 0x00000007)						//	mode8(n) = n % 8
#define mod8_flag(n) ((n & 0x00000007) == 0 ? 0 : 1)	//	mode_flag(n) = (n % 8 == 0 ? 0 : 1)

bool _get_bit(const ui8& c, int bit_index);
void _set_bit(ui8& c, int bit_index);
void _reset_bit(ui8& c, int bit_index);

template <int MAX_SIZE>
class BitSet
{
public:
	static const int FLAG_SIZE = div8(MAX_SIZE) + mod8_flag(MAX_SIZE);

	BitSet();

	void	set(int pos)	{ _set_bit	(_get_flag(pos), mod8(pos));		}
	void	set()			{ ::memset(m_flag, 0xFF, sizeof(m_flag));		}
	void	reset(int pos)	{ _reset_bit(_get_flag(pos), mod8(pos));		}
	void	reset()			{ ::memset(m_flag, 0, sizeof(m_flag));			}
	void	flip(int pos)	{ _flip_bit	(_get_flag(pos), mod8(pos));		}
	void	clear()			{ ::memset(m_flag, 0, sizeof(m_flag)); }
	bool	dirty() const	{ for (int i = 0; i < FLAG_SIZE; ++i) { if (m_flag[i] != 0) return true; } return false; }
	void	assign(const ui8* flags, int sizeofFlag);
	const ui8*	flags() const { return m_flag; }
	ui8*	lags() { return m_flag; }

	bool operator[](int pos) const { return _get_bit(_get_flag_const(pos), mod8(pos));	}

	template <typename T> void map(T& s) { for (int i = 0; i < FLAG_SIZE; ++i) s << m_flag[i]; }

private:
	ui8&	_get_flag(int pos);
	const ui8& _get_flag_const(int pos) const;

private:
	ui8	m_flag[FLAG_SIZE];
};

//for transform.
static const unsigned char ONE = 0x80;

inline bool _get_bit(const ui8& c, int bit_index)
{
	ui8 v = (c & (ONE >> bit_index));
	return (v != 0);
}

inline void _set_bit(ui8& c, int bit_index)
{
	c = (c  | (ONE >> bit_index));
}

inline void _reset_bit(ui8& c, int bit_index)
{
	c = (c  & (~(ONE >> bit_index)));
}

inline void _flip_bit(ui8& c, int bit_index)
{
	if (_get_bit(c, bit_index))
		_reset_bit(c, bit_index);
	else
		_set_bit(c, bit_index);
}

template <int MAX_SIZE>
BitSet<MAX_SIZE>::BitSet()
{
	::memset(m_flag, 0, sizeof(m_flag));
}

template <int MAX_SIZE>
ui8& BitSet<MAX_SIZE>::_get_flag( int pos )
{
	if (pos < 0 || pos >= MAX_SIZE)
	{
		assert(0);
		return m_flag[0];
	}
	int flagIndex	= div8(pos);
	return m_flag[flagIndex];
}

template <int MAX_SIZE>
const ui8& BitSet<MAX_SIZE>::_get_flag_const( int pos ) const
{
	if (pos < 0 || pos >= MAX_SIZE)
	{
		assert(0);
		return m_flag[0];
	}
	int flagIndex	= div8(pos);
	return m_flag[flagIndex];
}

template <int MAX_SIZE>
void BitSet<MAX_SIZE>::assign(const ui8* flags, int sizeofData) 
{ 
	if (sizeofData <= static_cast<int>(sizeof(m_flag))) 
		::memcpy(m_flag, flags, sizeofData); 
	else 
		assert(false); 
}

}

#endif


