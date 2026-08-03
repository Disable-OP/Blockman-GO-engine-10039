#include "NoiseGeneratorOctaves.h"
#include "NoiseGeneratorPerlin.h"

#include "Util/Random.h"

namespace BLOCKMAN
{
NoiseGeneratorOctaves::NoiseGeneratorOctaves(Random* pRand, int octaves)
	: m_octaves(octaves)
{
	m_perlinGenCollection = (NoiseGeneratorPerlin**)LordMalloc(sizeof(NoiseGeneratorPerlin*) * m_octaves);
	for (int i = 0; i < m_octaves; ++i)
	{
		m_perlinGenCollection[i] = LordNew NoiseGeneratorPerlin(pRand);
	}
}

NoiseGeneratorOctaves::~NoiseGeneratorOctaves()
{
	for (int i = 0; i < m_octaves; ++i)
		LordSafeDelete(m_perlinGenCollection[i]);

	LordSafeFree(m_perlinGenCollection);
	m_octaves = 0;
}

/**
* pars:(par2,3,4=noiseOffset ; so that adjacent noise segments connect) (pars5,6,7=x,y,zArraySize),(pars8,10,12 =
* x,y,z noiseScale)
*/
double* NoiseGeneratorOctaves::generateNoiseOctaves(double* pArray,
	int xOffset, int yOffset, int zOffset,
	int xSize, int ySize, int zSize,
	double xScale, double yScale, double zScale)
{
	if (pArray == NULL)
	{
		pArray = (double*)LordMalloc(sizeof(double) * xSize * ySize * zSize);
		memset(pArray, 0, sizeof(double) * xSize * ySize * zSize);
	}
	else
	{
		for (int i = 0; i < xSize * ySize * zSize; ++i)
		{
			pArray[i] = 0.0;
		}
	}

	double waveLen = 1.0;
	for (int i = 0; i < m_octaves; ++i)
	{
		double d0 = (double)xOffset * waveLen * xScale;
		double d1 = (double)yOffset * waveLen * yScale;
		double d2 = (double)zOffset * waveLen * zScale;
		i64 k = (i64)Math::Floor(d0);
		i64 l = (i64)Math::Floor(d2);
		d0 -= (double)k;
		d2 -= (double)l;
		k %= 0x1000000LL;
		l %= 0x1000000LL;
		d0 += (double)k;
		d2 += (double)l;

		m_perlinGenCollection[i]->populateNoiseArray(pArray,
			d0, d1, d2,
			xSize, ySize, zSize,
			xScale * waveLen, yScale * waveLen, zScale * waveLen,
			waveLen);

		waveLen /= 2.0;
	}

	return pArray;
}

double* NoiseGeneratorOctaves::generateNoiseOctaves(double* pArray, int xOffset, int zOffset, int xSize, int zSize, double xScale, double zScale)
{
	return generateNoiseOctaves(pArray, xOffset, 10, zOffset, xSize, 1, zSize, xScale, 1.0, zScale);
}

}