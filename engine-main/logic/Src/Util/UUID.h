/********************************************************************
filename: 	UUID.h
file path:	dev\client\Src\Blockman\Util

version:	1
author:		ajohn
company:	supernano
date:		2017-2-14
*********************************************************************/
#ifndef __UUID_HEADER__
#define __UUID_HEADER__

#include "Core.h"
#include "Util/StringUtil.h"
#include "Util/AssertX.h"

using namespace LORD;

namespace BLOCKMAN
{

class UUID
{
private:
	/** Explicit serialVersionUID for interoperability. */
	const static i64 serialVersionUID = -4856846361193249489L;

	/* The most significant 64 bits of this UUID. */
	i64 mostSigBits;

	/* The least significant 64 bits of this UUID. */
	i64 leastSigBits;

	/* Private constructor which uses a byte array to construct the new UUID. */
	UUID(i8* data)
	{
		i64 msb = 0;
		i64 lsb = 0;

		for (int i = 0; i < 8; i++)
			msb = (msb << 8) | (data[i] & 0xff);
		for (int i = 8; i < 16; i++)
			lsb = (lsb << 8) | (data[i] & 0xff);
		mostSigBits = msb;
		leastSigBits = lsb;
	}

public:
	UUID()
	{}

	UUID(i64 mostSigBits, i64 leastSigBits)
		: mostSigBits(mostSigBits)
		, leastSigBits(leastSigBits)
	{
	}

	static UUID randomUUID()
	{
		i8 randomBytes[16];
		int* pInt = (int*)randomBytes;
		for (int i = 0; i < 4; ++i)
		{
			*pInt = rand();
			pInt++;
		}

		randomBytes[6] &= 0x0f;  /* clear version        */
		randomBytes[6] |= 0x40;  /* set to version 4     */
		randomBytes[8] &= 0x3f;  /* clear variant        */
		randomBytes[8] |= 0x80;  /* set to IETF variant  */
		return UUID(randomBytes);
	}

	static UUID fromString(const String& name)
	{
		StringArray components = StringUtil::Split(name, "-");
		LordAssert(components.size() == 5);

		i64 mostSigBits = StringUtil::ParseHex<i64>(components[0]);
		mostSigBits <<= 16;
		mostSigBits |= StringUtil::ParseHex<i64>(components[1]);
		mostSigBits <<= 16;
		mostSigBits |= StringUtil::ParseHex<i64>(components[2]);

		i64 leastSigBits = StringUtil::ParseHex<i64>(components[3]);
		leastSigBits <<= 48;
		leastSigBits |= StringUtil::ParseHex<i64>(components[4]);

		return UUID(mostSigBits, leastSigBits);
	}

	i64 getLeastSignificantBits() const { return leastSigBits; }
	i64 getMostSignificantBits() const { return mostSigBits; }

	int version() { return (int)((mostSigBits >> 12) & 0x0f); }
	int variant() { return (int)((leastSigBits >> (64 - (leastSigBits >> 62))) & (leastSigBits >> 63)); }

	i64 timestamp()
	{
		LordAssert(version() == 1);

		return (mostSigBits & 0x0FFFL) << 48
			| ((mostSigBits >> 16) & 0x0FFFFL) << 32
			| mostSigBits >> 32;
	}

	int clockSequence()
	{
		LordAssert(version() == 1);
		return (int)((leastSigBits & 0x3FFF000000000000L) >> 48);
	}

	i64 node()
	{
		LordAssert(version() == 1);
		return leastSigBits & 0x0000FFFFFFFFFFFFL;
	}

	String toString()
	{
		return (digits(mostSigBits >> 32, 8) + "-" +
			digits(mostSigBits >> 16, 4) + "-" +
			digits(mostSigBits, 4) + "-" +
			digits(leastSigBits >> 48, 4) + "-" +
			digits(leastSigBits, 12));
	}

	/** Returns val represented by the specified number of hex digits. */
	static String digits(i64 val, int digits)
	{
		i64 hi = i64(1L) << (digits * 4);
		return StringUtil::Format("%0xll", hi | (val & (hi - 1)));
	}

	/** Returns a hash code for this */
	int hashCode() { i64 hilo = mostSigBits ^ leastSigBits; return ((int)(hilo >> 32)) ^ (int)hilo; }

	bool operator == (const UUID& rhs) const
	{
		return (mostSigBits == rhs.mostSigBits &&
			leastSigBits == rhs.leastSigBits);
	}

	bool operator > (const UUID& rhs) const
	{
		if (mostSigBits > rhs.mostSigBits) {
			return true;
		}
		else if (mostSigBits == rhs.mostSigBits) {
			return leastSigBits > rhs.leastSigBits;
		}
		else {
			return false;
		}
	}
	
	bool operator < (const UUID& rhs) const
	{
		if (mostSigBits < rhs.mostSigBits) {
			return true;
		}
		else if (mostSigBits == rhs.mostSigBits) {
			return leastSigBits < rhs.leastSigBits;
		}
		else {
			return false;
		}
	}
};
}
#endif