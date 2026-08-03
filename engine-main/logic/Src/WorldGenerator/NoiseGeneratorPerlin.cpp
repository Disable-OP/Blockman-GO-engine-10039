#include "NoiseGeneratorPerlin.h"

#include "Util/Random.h"

namespace BLOCKMAN
{

NoiseGeneratorPerlin::NoiseGeneratorPerlin(Random* pRand)
{
	LordAssert(pRand);

	m_permutations = (int*)LordMalloc(sizeof(int) * 512);
	m_x = pRand->nextDouble() * 256.0;
	m_y = pRand->nextDouble() * 256.0;
	m_z = pRand->nextDouble() * 256.0;

	int i;
	for (i = 0; i < 256; ++i)
	{
		m_permutations[i] = i;
	}

	for (i = 0; i < 256; ++i)
	{
		int index = pRand->nextInt(256 - i) + i;
		int temp = m_permutations[i];
		m_permutations[i] = m_permutations[index];
		m_permutations[index] = temp;
		m_permutations[i + 256] = m_permutations[i];
	}
}

NoiseGeneratorPerlin::~NoiseGeneratorPerlin()
{
	LordSafeFree(m_permutations);
}

double NoiseGeneratorPerlin::lerp(double s, double a, double b)
{
	return a + s * (b - a);
}

double NoiseGeneratorPerlin::curve(int s, double a, double b)
{
	int var6 = s & 15;
	double var7 = (double)(1 - ((var6 & 8) >> 3)) * a;
	double var9 = var6 < 4 ? 0.0 : (var6 != 12 && var6 != 14 ? b : a);
	return ((var6 & 1) == 0 ? var7 : -var7) + ((var6 & 2) == 0 ? var9 : -var9);
}

double NoiseGeneratorPerlin::grad(int hash, double x, double y, double z)
{
	int h = hash & 15;
	double u = h < 8 ? x : y;
	double v = h < 4 ? y : (h != 12 && h != 14 ? z : x);
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

/*
	classic noise and simplex noise 
	http://webstaff.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
*/
void NoiseGeneratorPerlin::populateNoiseArray(double* pArrayOfDouble,
	double xOffset, double yOffset, double zOffset,
	int xSize, int ySize, int zSize,
	double xScale, double yScale, double zScale,
	double noiseScale)
{


	if (ySize == 1)
	{
		int index = 0;
		double scale = 1.0 / noiseScale;

		for (int ix = 0; ix < xSize; ++ix)
		{
			double tx = xOffset + (double)ix * xScale + m_x;
			int X = (int)tx;

			if (tx < (double)X)
			{
				--X;
			}

			int idxX = X & 255;
			tx -= (double)X;
			//6t^5 - 15t^4 + 10t^3
			double sx = tx * tx * tx * (tx * (tx * 6.0 - 15.0) + 10.0);

			for (int iz = 0; iz < zSize; ++iz)
			{
				double tz = zOffset + (double)iz * zScale + m_z;
				int Z = (int)tz;

				if (tz < (double)Z)
				{
					--Z;
				}

				int idxZ = Z & 255;
				tz -= (double)Z;
				//6t^5 - 15t^4 + 10t^3
				double sz = tz * tz * tz * (tz * (tz * 6.0 - 15.0) + 10.0);
				int permutation1 = m_permutations[idxX] + 0;
				int permutation2 = m_permutations[permutation1] + idxZ;
				int permutation3 = m_permutations[idxX + 1] + 0;
				int permutation4 = m_permutations[permutation3] + idxZ;
				
				// interpolate along x
				double n1 = lerp(sx, curve(m_permutations[permutation2], tx, tz), grad(m_permutations[permutation4], tx - 1.0, 0.0, tz));
				double n2 = lerp(sx, grad(m_permutations[permutation2 + 1], tx, 0.0, tz - 1.0), grad(m_permutations[permutation4 + 1], tx - 1.0, 0.0, tz - 1.0));
				// interpolate along z
				double n3 = lerp(sz, n1, n2);	
							
				pArrayOfDouble[index++] += n3 * scale;
			}
		}
	}
	else
	{
		int index = 0;
		double scale = 1.0 / noiseScale;
		int var22 = -1;
		double n1 = 0.0;
		double n2 = 0.0;
		double n3 = 0.0;
		double n4 = 0.0;

		for (int ix = 0; ix < xSize; ++ix)
		{
			double tx = xOffset + (double)ix * xScale + m_x;
			int X = (int)tx;

			if (tx < (double)X)
			{
				--X;
			}

			int idxX = X & 255;
			tx -= (double)X;
			double sx = tx * tx * tx * (tx * (tx * 6.0 - 15.0) + 10.0);

			for (int iz = 0; iz < zSize; ++iz)
			{
				double tz = zOffset + (double)iz * zScale + m_z;
				int Z = (int)tz;

				if (tz < (double)Z)
				{
					--Z;
				}

				int idxZ = Z & 255;
				tz -= (double)Z;
				double sz = tz * tz * tz * (tz * (tz * 6.0 - 15.0) + 10.0);

				for (int iy = 0; iy < ySize; ++iy)
				{
					double ty = yOffset + (double)iy * yScale + m_y;
					int Y = (int)ty;

					if (ty < (double)Y)
					{
						--Y;
					}

					int idxY = Y & 255;
					ty -= (double)Y;
					double sy = ty * ty * ty * (ty * (ty * 6.0 - 15.0) + 10.0);

					if (iy == 0 || idxY != var22)
					{
						var22 = idxY;
						int permutation1 = m_permutations[idxX] + idxY;
						int permutation2 = m_permutations[permutation1] + idxZ;
						int permutation3 = m_permutations[permutation1 + 1] + idxZ;
						int permutation4 = m_permutations[idxX + 1] + idxY;
						int permutation5 = m_permutations[permutation4] + idxZ;
						int permutation6 = m_permutations[permutation4 + 1] + idxZ;
						// interpolate along x
						n1 = lerp(sx, grad(m_permutations[permutation2], tx, ty, tz), grad(m_permutations[permutation5], tx - 1.0, ty, tz));
						n2 = lerp(sx, grad(m_permutations[permutation3], tx, ty - 1.0, tz), grad(m_permutations[permutation6], tx - 1.0, ty - 1.0, tz));
						n3 = lerp(sx, grad(m_permutations[permutation2 + 1], tx, ty, tz - 1.0), grad(m_permutations[permutation5 + 1], tx - 1.0, ty, tz - 1.0));
						n4 = lerp(sx, grad(m_permutations[permutation3 + 1], tx, ty - 1.0, tz - 1.0), grad(m_permutations[permutation6 + 1], tx - 1.0, ty - 1.0, tz - 1.0));
					}

					// interpolate along y
					double n5 = lerp(sy, n1, n2);
					double n6 = lerp(sy, n3, n4);
					// interpolate along z
					double n7 = lerp(sz, n5, n6);
					
					pArrayOfDouble[index++] += n7 * scale;
				}
			}
		}
	}
}


}