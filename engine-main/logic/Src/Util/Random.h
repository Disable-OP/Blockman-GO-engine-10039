/********************************************************************
filename: 	Random.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __RANDOM_HEADER__
#define __RANDOM_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

// not thread safe!
class Random : public ObjectAlloc
{
public:
	Random();
	Random(i64 seed);

	static i64 seedUniquifier();

	i64		initialScramble(i64 seed);
	void	setSeed(i64 seed);
	int		next(int bits);
	i64		internalNextLong(i64 origin, i64 bound);
	int		internalNextInt(int origin, int bound);
	int		nextIntRange(int min, int max);
	int		nextInt();
	int		nextInt(int bound);
	i64		nextLong();
	bool	nextBool();
	float	nextFloat();
	double	nextDouble();
	double	nextGaussian();
	Vector3 nextVector();
	Vector3 nextGaussianVec();

protected:
	const static i64 s_multiplier = 0x5DEECE66DL;
	const static i64 s_addend = 0xBL;
	const static i64 s_mask = (1LL << 48) - 1;
	const static double s_DOUBLE_UNIT; // volotile value must init in cpp.

protected:
	i64 m_seed = 0;		
	double m_nextNextGaussian = 0.0;
	bool m_haveNextNextGaussian = false;
};

}

#endif