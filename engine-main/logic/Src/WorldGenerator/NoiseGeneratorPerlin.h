/********************************************************************
filename: 	NoiseGeneratorPerlin.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-4
*********************************************************************/
#ifndef __NOISE_GENERATOR_PERLIN_HEADER__
#define __NOISE_GENERATOR_PERLIN_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class Random;

class NoiseGeneratorPerlin : public ObjectAlloc
{
public:
	NoiseGeneratorPerlin(Random* pRand);
	~NoiseGeneratorPerlin();

	inline double lerp(double s, double a, double b);

	inline double curve(int s, double a, double b);

	//gradient
	double grad(int hash, double x, double y, double z);

	/**
	* pars: noiseArray , xOffset , yOffset , zOffset , xSize , ySize , zSize , xScale, yScale , zScale , noiseScale.
	* noiseArray should be xSize*ySize*zSize in size
	*/
	void populateNoiseArray(double* pArrayOfDouble,
		double xOffset, double yOffset, double zOffset,
		int xSize, int ySize, int zSize,
		double xScale, double yScale, double zScale,
		double noiseScale);

protected:
	int* m_permutations = nullptr;
	double m_x = 0.f;
	double m_y = 0.f;
	double m_z = 0.f;
};

}

#endif