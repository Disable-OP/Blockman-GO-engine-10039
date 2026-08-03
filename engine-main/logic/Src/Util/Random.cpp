#include "Random.h"

namespace BLOCKMAN
{

const double Random::s_DOUBLE_UNIT = (1.0 / (1LL << 53));

Random::Random()
	: m_haveNextNextGaussian(false)
	, m_nextNextGaussian(0.0)
{
	m_seed = initialScramble(seedUniquifier() ^ LORD::Time::Instance()->getMicrosecondsCPU());
}

Random::Random(i64 seed)
	: m_haveNextNextGaussian(false)
	, m_nextNextGaussian(0.0)
{
	m_seed = initialScramble(seed);
}

i64 Random::seedUniquifier()
{
	static i64 iseedUniquifier = 8682522807148012L;

	for (;;)
	{
		i64 current = iseedUniquifier;
		i64 next = current * 181783497276652981L;

		if (current != next)
		{
			iseedUniquifier = next;
			return next;
		}
	}
}

i64 Random::initialScramble(i64 seed)
{
	return (seed ^ s_multiplier) & s_mask;
}

void Random::setSeed(i64 seed)
{
	m_seed = initialScramble(seed);
	m_haveNextNextGaussian = false;
}

int Random::next(int bits)
{
	i64 oldseed, nextseed;
	i64 seed = m_seed;

	oldseed = seed;
	nextseed = (oldseed * s_multiplier + s_addend) & s_mask;
	m_seed = nextseed;
	return (int)((ui64)nextseed >> (48 - bits));
}

i64 Random::internalNextLong(i64 origin, i64 bound)
{
	i64 r = nextLong();

	if (origin < bound)
	{
		i64 n = bound - origin, m = n - 1;
		if ((n & m) == 0)  // power of two
			r = (r & m) + origin;
		else if (n > 0)
		{
			for (i64 u = (i64)((ui64)r >> 1);            // ensure nonnegative
			u + m - (r = u % n) < 0;    // rejection check
				u = (i64)((ui64)nextLong() >> 1)) // retry
				;
			r += origin;
		}
		else
		{              // range not representable as i64
			while (r < origin || r >= bound)
				r = nextLong();
		}
	}
	return r;
}

int Random::internalNextInt(int origin, int bound)
{
	if (origin < bound)
	{
		int n = bound - origin;
		if (n > 0)
		{
			return nextInt(n) + origin;
		}
		else
		{  // range not representable as int
			int r;
			do
			{
				r = nextInt();
			} while (r < origin || r >= bound);

			return r;
		}
	}
	else
	{
		return nextInt();
	}
}

int Random::nextInt()
{
	return next(32);
}

int Random::nextInt(int bound)
{
	LordAssertX(bound > 0, "bound must be positive");
	
	int r = next(31);
	int m = bound - 1;
	if ((bound & m) == 0)  // i.e., bound is a power of 2
		r = (int)((bound * (i64)r) >> 31);
	else 
	{
		for (int u = r;	u - (r = u % bound) + m < 0; u = next(31))
			;
	}
	return r;
}

int Random::nextIntRange(int min, int max)
{
	return min >= max ? min : nextInt(max - min + 1) + min;
}

i64 Random::nextLong()
{
	// it's okay that the bottom word remains signed.
	return ((i64)(next(32)) << 32) + next(32);
}

bool Random::nextBool()
{
	return next(1) != 0;
}

float Random::nextFloat()
{
	return next(24) / ((float)(1 << 24));
}

Vector3 Random::nextVector()
{
	Vector3 result;
	result.x = nextFloat();
	result.y = nextFloat();
	result.z = nextFloat();

	return result;
}

Vector3 Random::nextGaussianVec()
{
	Vector3 result;
	result.x = float(nextGaussian());
	result.y = float(nextGaussian());
	result.z = float(nextGaussian());

	return result;
}

double Random::nextDouble()
{
	return (((i64)(next(26)) << 27) + next(27)) * s_DOUBLE_UNIT;
}

double Random::nextGaussian()
{
	// See Knuth, ACP, Section 3.4.1 Algorithm C.
	if (m_haveNextNextGaussian)
	{
		m_haveNextNextGaussian = false;
		return m_nextNextGaussian;
	}
	else
	{
		double v1, v2, s;
		do
		{
			v1 = 2 * nextDouble() - 1; // between -1 and 1
			v2 = 2 * nextDouble() - 1; // between -1 and 1
			s = v1 * v1 + v2 * v2;
		} while (s >= 1 || s == 0);

		double multiplier = Math::Sqrt(-2 * Math::Log(s) / s);
		m_nextNextGaussian = v2 * multiplier;
		m_haveNextNextGaussian = true;
		return v1 * multiplier;
	}
}

}