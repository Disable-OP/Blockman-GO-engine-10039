/********************************************************************
filename: 	NoiseGeneratorOctaves.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-3
*********************************************************************/
#ifndef __NOISE_GENERATOR_OCTAVES_HEADER__
#define __NOISE_GENERATOR_OCTAVES_HEADER__

#include "Core.h"
using namespace LORD;

namespace BLOCKMAN
{

class NoiseGeneratorPerlin;
class Random;

class NoiseGeneratorOctaves : public ObjectAlloc
{
public:
	NoiseGeneratorOctaves(Random* pRand, int octaves);

	~NoiseGeneratorOctaves();

	/** pars:(par2,3,4=noiseOffset ; so that adjacent noise segments connect) (pars5,6,7=x,y,zArraySize),(pars8,10,12 = x,y,z noiseScale) */
	double* generateNoiseOctaves(double* pArray, int xOffset, int yOffset, int zOffset,	int xSize, int ySize, int zSize, double xScale, double yScale, double zScale);
	
	/** Bouncer function to the main one with some default arguments. */
	double* generateNoiseOctaves(double* pArray, int xOffset, int zOffset, int xSize, int zSize, double xScale, double zScale);

protected:
	/** Collection of noise generation functions.  Output is combined to produce different octaves of noise. */
	NoiseGeneratorPerlin** m_perlinGenCollection;
	int m_octaves = 0;
};

}

#endif