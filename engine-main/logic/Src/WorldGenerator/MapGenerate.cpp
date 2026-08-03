#include "MapGenerate.h"
#include "StructureStart.h"
#include "StructureComponent.h"
#include "BiomeGen.h"

#include "World/World.h"
#include "World/WorldChunkManager.h"
#include "Util/Random.h"
#include "Util/ChunkCoordIntPair.h"

namespace BLOCKMAN
{

MapGenBase::MapGenBase()
	: m_range(8)
{
}

MapGenBase::~MapGenBase()
{
}

void MapGenBase::generate(World* pWorld, int x, int z, i8* pArray)
{
	int range = m_range;
	m_pWorld = pWorld;
	m_rand.setSeed(pWorld->getSeed());
	i64 var7 = m_rand.nextLong();
	i64 var9 = m_rand.nextLong();

	for (int tx = x - range; tx <= x + range; ++tx)
	{
		for (int tz = z - range; tz <= z + range; ++tz)
		{
			i64 var13 = (i64)tx * var7;
			i64 var15 = (i64)tz * var9;
			m_rand.setSeed(var13 ^ var15 ^ pWorld->getSeed());
			recursiveGenerate(pWorld, tx, tz, x, z, pArray);
		}
	}
}


MapGenStructure::MapGenStructure()
{}

void MapGenStructure::recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArrayOfByte)
{
	if (m_structureMap.find(ChunkCoordIntPair::chunkXZ2Int(x, z)) != m_structureMap.end())
		return;

	m_rand.nextInt();

	try
	{
		if (canSpawnStructureAtCoords(x, z))
		{
			StructureStart* pss = getStructureStart(x, z);
			m_structureMap.insert(std::make_pair(ChunkCoordIntPair::chunkXZ2Int(x, z), pss));
		}
	}
	catch (...)
	{
		LordException("Exception preparing structure feature");
	}
}

bool MapGenStructure::generateStructuresInChunk(World* pWorld, Random& rand, int chunkX, int chunkZ)
{
	int minx = (chunkX << 4) + 8;
	int minz = (chunkZ << 4) + 8;
	bool generated = false;

	for (StructureMap::iterator it = m_structureMap.begin(); it != m_structureMap.end(); ++it)
	{
		StructureStart* pss = it->second;
		if (pss->isSizeableStructure() &&
			pss->getBoundingBox().intersectsWith(minx, minz, minx + 15, minz + 15))
		{
			pss->generateStructure(pWorld, rand, StructureBB(minx, minz, minx + 15, minz + 15));
			generated = true;
		}
	}

	return generated;
}

bool MapGenStructure::hasStructureAt(int x, int y, int z)
{
	for (StructureMap::iterator it = m_structureMap.begin(); it != m_structureMap.end(); ++it)
	{
		StructureStart* pss = it->second;

		if (!(pss->isSizeableStructure() && pss->getBoundingBox().intersectsWith(x, z, x, z)))
			continue;

		StructureStart::ComponentList& components = pss->getComponents();
		for (StructureStart::ComponentList::iterator jt = components.begin(); jt != components.end(); ++jt)
		{
			StructureComponent* pComponent = *jt;
			if (pComponent->getBoundingBox().isVecInside(x, y, z))
				return true;
		}
	}

	return false;
}

bool MapGenStructure::func_142038_b(int x, int y, int z)
{
	StructureStart* pss = NULL;
	for (StructureMap::iterator it = m_structureMap.begin(); it != m_structureMap.end(); ++it)
	{
		pss = it->second;
		if (pss->isSizeableStructure())
			break;
	}

	if (!pss)
		return false;

	return pss->getBoundingBox().intersectsWith(x, z, x, z);
}

ChunkPosition MapGenStructure::getNearestInstance(World* pWorld, int x, int y, int z)
{
	m_pWorld = pWorld;
	m_rand.setSeed(pWorld->getSeed());
	i64 var5 = m_rand.nextLong();
	i64 var7 = m_rand.nextLong();
	i64 var9 = (i64)(x >> 4) * var5;
	i64 var11 = (i64)(z >> 4) * var7;
	m_rand.setSeed(var9 ^ var11 ^ pWorld->getSeed());
	recursiveGenerate(pWorld, x >> 4, z >> 4, 0, 0, NULL);
	double var13 = Math::MAX_DOUBLE;
	ChunkPosition result;
	ChunkPosition var19;
	int var20;
	int var21;
	int var22;
	double var23;

	for (StructureMap::iterator it = m_structureMap.begin(); it != m_structureMap.end(); ++it)
	{
		StructureStart* pss = it->second;
		if (!pss || !pss->isSizeableStructure())
			continue;

		StructureComponent* pComponent = pss->getComponents().front();
		if (!pComponent)
			continue;

		var19 = pComponent->getCenter();
		var20 = var19.m_x - x;
		var21 = var19.m_y - y;
		var22 = var19.m_z - z;
		var23 = (double)(var20 * var20 + var21 * var21 + var22 * var22);

		if (var23 < var13)
		{
			var13 = var23;
			result = var19;
		}
	}

	return result;
}

MapGenMineshaft::MapGenMineshaft()
{
	m_probability = 0.01;
}

//MapGenerateMineshaft::MapGenerateMineshaft(const StringArray& par1Map)
//{
//	Iterator var2 = par1Map.entrySet().iterator();
//
//	while (var2.hasNext())
//	{
//		Entry var3 = (Entry)var2.next();
//
//		if (((String)var3.getKey()).equals("chance"))
//		{
//			this.field_82673_e = MathHelper.parseDoubleWithDefault((String)var3.getValue(), this.field_82673_e);
//		}
//	}
//}

bool MapGenMineshaft::canSpawnStructureAtCoords(int x, int z)
{
	return m_rand.nextDouble() < m_probability &&
		m_rand.nextInt(80) < Math::Max(Math::Abs(x), Math::Abs(z));
}

StructureStart* MapGenMineshaft::getStructureStart(int x, int z)
{
	return LordNew StructureMineshaftStart(m_pWorld, m_rand, x, z);
}

void MapGenCaves::generateCaveNode(i64 seed, int centerX, int centerZ, i8* pArr, double x, double y, double z, float par12, float radian1, float radian2, int begin, int end, double scaleY)
{
	//par12影响洞穴的大小
	double midX = (double)(centerX * 16 + 8);
	double midZ = (double)(centerZ * 16 + 8);
	float var23 = 0.0F;
	float var24 = 0.0F;
	Random rand(seed);

	if (end <= 0)
	{
		int var26 = m_range * 16 - 16;
		end = var26 - rand.nextInt(var26 / 4);
	}

	bool isLargeCaveNode = false;

	if (begin == -1)
	{
		begin = end / 2;
		isLargeCaveNode = true;
	}

	//end*1/4 ~ end*3/4
	int target = rand.nextInt(end / 2) + end / 4;
	bool flag = rand.nextInt(6) == 0;

	for (; begin < end; ++begin)
	{
		double offset = 1.5 + (double)(Math::Sin_tab((float)begin * (float)Math::PI / (float)end) * par12 * 1.0F);
		double offsetY = offset * scaleY;		
		float beta = Math::Cos_tab(radian2);
		float betaY = Math::Sin_tab(radian2);
		x += (double)(Math::Cos_tab(radian1) * beta);
		y += (double)betaY;
		z += (double)(Math::Sin_tab(radian1) * beta);

		if (flag)
		{
			radian2 *= 0.92F;
		}
		else
		{
			radian2 *= 0.7F;
		}

		radian2 += var24 * 0.1F;
		radian1 += var23 * 0.1F;
		var24 *= 0.9F;
		var23 *= 0.75F;
		float n1 = rand.nextFloat();
		float n2 = rand.nextFloat();
		float n3 = rand.nextFloat();
		var24 += (n1 - n2) * n3 * 2.0F;
		n1 = rand.nextFloat();
		n2 = rand.nextFloat();
		n3 = rand.nextFloat();
		var23 += (n1 - n2) * n3 * 4.0F;

		if (!isLargeCaveNode && begin == target && par12 > 1.0F && end > 0)
		{
			i64 nextseed = rand.nextLong();
			float nextfloat = rand.nextFloat();
			generateCaveNode(nextseed, centerX, centerZ, pArr, x, y, z, nextfloat * 0.5F + 0.5F, radian1 - ((float)Math::PI / 2.F), radian2 / 3.0F, begin, end, 1.0);
			nextseed = rand.nextLong();
			nextfloat = rand.nextFloat();
			generateCaveNode(nextseed, centerX, centerZ, pArr, x, y, z, nextfloat * 0.5F + 0.5F, radian1 + ((float)Math::PI / 2.F), radian2 / 3.0F, begin, end, 1.0);
			return;
		}

		if (isLargeCaveNode || rand.nextInt(4) != 0)
		{			
			double disX = x - midX;
			double disZ = z - midZ;
			double var39 = (double)(end - begin);
			double var41 = (double)(par12 + 2.0F + 16.0F);

			if (disX * disX + disZ * disZ - var39 * var39 > var41 * var41)
			{
				return;
			}

			if (x >= midX - 16.0 - offset * 2.0 && z >= midZ - 16.0 - offset * 2.0 && x <= midX + 16.0 + offset * 2.0 && z <= midZ + 16.0 + offset * 2.0)
			{
				int minX = int(Math::Floor(x - offset) - centerX * 16 - 1);
				int maxX = int(Math::Floor(x + offset) - centerX * 16 + 1);
				int minY = int(Math::Floor(y - offsetY) - 1);
				int maxY = int(Math::Floor(y + offsetY) + 1);
				int minZ = int(Math::Floor(z - offset) - centerZ * 16 - 1);
				int maxZ = int(Math::Floor(z + offset) - centerZ * 16 + 1);

				if (minX < 0)
				{
					minX = 0;
				}

				if (maxX > 16)
				{
					maxX = 16;
				}

				if (minY < 1)
				{
					minY = 1;
				}

				if (maxY > 120)
				{
					maxY = 120;
				}

				if (minZ < 0)
				{
					minZ = 0;
				}

				if (maxZ > 16)
				{
					maxZ = 16;
				}

				//check
				bool hasWater = false;
				for (int ix = minX; !hasWater && ix < maxX; ++ix)
				{
					for (int iz = minZ; !hasWater && iz < maxZ; ++iz)
					{
						for (int iy = maxY + 1; !hasWater && iy >= minY - 1; --iy)
						{
							int index = (ix * 16 + iz) * 128 + iy;

							if (iy >= 0 && iy < 128)
							{
								if (pArr[index] == BLOCK_ID_WATERMOVING || pArr[index] == BLOCK_ID_WATERSTILL)
								{
									hasWater = true;
								}

								if (iy != minY - 1 && ix != minX && ix != maxX - 1 && iz != minZ && iz != maxZ - 1)
								{
									iy = minY;
								}
							}
						}
					}
				}

				//replace block
				if (!hasWater)
				{
					for (int ix = minX; ix < maxX; ++ix)
					{
						double var59 = ((double)(ix + centerX * 16) + 0.5 - x) / offset;

						for (int iz = minZ; iz < maxZ; ++iz)
						{
							double var46 = ((double)(iz + centerZ * 16) + 0.5 - z) / offset;
							int index = (ix * 16 + iz) * 128 + maxY;
							bool hasGrass = false;

							if (var59 * var59 + var46 * var46 < 1.0)
							{
								for (int iy = maxY - 1; iy >= minY; --iy)
								{
									double var51 = ((double)iy + 0.5 - y) / offsetY;

									if (var51 > -0.7 && var59 * var59 + var51 * var51 + var46 * var46 < 1.0)
									{
										i8 blockID = pArr[index];

										if (blockID == BLOCK_ID_GRASS)
										{
											hasGrass = true;
										}

										if (blockID == BLOCK_ID_STONE || blockID == BLOCK_ID_DIRT || blockID == BLOCK_ID_GRASS)
										{
											if (iy < 10)
											{
												pArr[index] = (i8)BLOCK_ID_LAVAMOVING;
											}
											else
											{
												pArr[index] = 0;

												if (hasGrass && pArr[index - 1] == BLOCK_ID_DIRT)
												{
													pArr[index - 1] = m_pWorld->getBiomeGenForCoords(ix + centerX * 16, iz + centerZ * 16)->m_topBlock;
												}
											}
										}
									}

									--index;
								}
							}
						}
					}

					if (isLargeCaveNode)
					{
						break;
					}
				}
			}
		}
	}
}

void MapGenCaves::recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArr)
{
	int caveBatch = m_rand.nextInt(m_rand.nextInt(m_rand.nextInt(40) + 1) + 1);

	if (m_rand.nextInt(15) != 0)
	{
		caveBatch = 0;
	}

	for (int i = 0; i < caveBatch; ++i)
	{
		double tx = (double)(x * 16 + m_rand.nextInt(16));
		double ty = (double)m_rand.nextInt(m_rand.nextInt(120) + 8);
		double tz = (double)(z * 16 + m_rand.nextInt(16));
		int caveNodeNum = 1;

		if (m_rand.nextInt(4) == 0)
		{
			generateLargeCaveNode(m_rand.nextLong(), centerX, centerZ, pArr, tx, ty, tz);
			caveNodeNum += m_rand.nextInt(4);
		}

		for (int j = 0; j < caveNodeNum; ++j)
		{
			float radian1 = m_rand.nextFloat() * (float)Math::PI * 2.0F;
			float radian2 = (m_rand.nextFloat() - 0.5F) * 2.0F / 8.0F;
			float n1 = m_rand.nextFloat();
			float n2 = m_rand.nextFloat();
			float var19 = n1 * 2.0F + n2;

			if (m_rand.nextInt(10) == 0)
			{
				n1 = m_rand.nextFloat();
				n2 = m_rand.nextFloat();
				var19 *= n1 * n2 * 3.0F + 1.0F;
			}
			//19,17,18
			generateCaveNode(m_rand.nextLong(), centerX, centerZ, pArr, tx, ty, tz, var19, radian1, radian2, 0, 0, 1.0);
		}
	}
}

void MapGenCavesHell::generateCaveNode(i64 seed, int centerX, int centerZ, i8* pArr, double x, double y, double z, float par12, float par13, float par14, int begin, int end, double par17)
{
	double midX = (double)(centerX * 16 + 8);
	double midZ = (double)(centerZ * 16 + 8);
	float var23 = 0.0F;
	float var24 = 0.0F;
	Random rand(seed);

	if (end <= 0)
	{
		int var26 = m_range * 16 - 16;
		end = var26 - rand.nextInt(var26 / 4);
	}

	bool flag = false;

	if (begin == -1)
	{
		begin = end / 2;
		flag = true;
	}

	int var27 = rand.nextInt(end / 2) + end / 4;
	bool flag2 = rand.nextInt(6) == 0;

	for (; begin < end; ++begin)
	{
		double offset = 1.5 + (double)(Math::Sin_tab((float)begin * (float)Math::PI / (float)end) * par12 * 1.0F);
		double offsetY = offset * par17;
		float var33 = Math::Cos_tab(par14);
		float var34 = Math::Sin_tab(par14);
		x += (double)(Math::Cos_tab(par13) * var33);
		y += (double)var34;
		z += (double)(Math::Sin_tab(par13) * var33);

		if (flag2)
		{
			par14 *= 0.92F;
		}
		else
		{
			par14 *= 0.7F;
		}

		par14 += var24 * 0.1F;
		par13 += var23 * 0.1F;
		var24 *= 0.9F;
		var23 *= 0.75F;
		float n1 = rand.nextFloat();
		float n2 = rand.nextFloat();
		float n3 = rand.nextFloat();
		var24 += (n1 - n2) * n3 * 2.0F;
		n1 = rand.nextFloat();
		n2 = rand.nextFloat();
		n3 = rand.nextFloat();
		var23 += (n1 - n2) * n3 * 4.0F;

		if (!flag && begin == var27 && par12 > 1.0F)
		{
			generateCaveNode(rand.nextLong(), centerX, centerZ, pArr, x, y, z, rand.nextFloat() * 0.5F + 0.5F, par13 - ((float)Math::PI / 2.F), par14 / 3.0F, begin, end, 1.0);
			generateCaveNode(rand.nextLong(), centerX, centerZ, pArr, x, y, z, rand.nextFloat() * 0.5F + 0.5F, par13 + ((float)Math::PI / 2.F), par14 / 3.0F, begin, end, 1.0);
			return;
		}

		if (flag || rand.nextInt(4) != 0)
		{
			double disX = x - midX;
			double disZ = z - midZ;
			double var39 = (double)(end - begin);
			double var41 = (double)(par12 + 2.0F + 16.0F);

			if (disX * disX + disZ * disZ - var39 * var39 > var41 * var41)
			{
				return;
			}

			if (x >= midX - 16.0 - offset * 2.0 && z >= midZ - 16.0 - offset * 2.0 && x <= midX + 16.0 + offset * 2.0 && z <= midZ + 16.0 + offset * 2.0)
			{
				int minX = int(Math::Floor(x - offset) - centerX * 16 - 1);
				int maxX = int(Math::Floor(x + offset) - centerX * 16 + 1);
				int minY = int(Math::Floor(y - offsetY) - 1);
				int maxY = int(Math::Floor(y + offsetY) + 1);
				int minZ = int(Math::Floor(z - offset) - centerZ * 16 - 1);
				int maxZ = int(Math::Floor(z + offset) - centerZ * 16 + 1);

				if (minX < 0)
				{
					minX = 0;
				}

				if (maxX > 16)
				{
					maxX = 16;
				}

				if (minY < 1)
				{
					minY = 1;
				}

				if (maxY > 120)
				{
					maxY = 120;
				}

				if (minZ < 0)
				{
					minZ = 0;
				}

				if (maxZ > 16)
				{
					maxZ = 16;
				}

				bool hasLava = false;

				for (int ix = minX; !hasLava && ix < maxX; ++ix)
				{
					for (int iz = minZ; !hasLava && iz < maxZ; ++iz)
					{
						for (int iy = maxY + 1; !hasLava && iy >= minY - 1; --iy)
						{
							int index = (ix * 16 + iz) * 128 + iy;

							if (iy >= 0 && iy < 128)
							{
								if (pArr[index] == BLOCK_ID_LAVAMOVING || pArr[index] == BLOCK_ID_LAVASTILL)
								{
									hasLava = true;
								}

								if (iy != minY - 1 && ix != minX && ix != maxX - 1 && iz != minZ && iz != maxZ - 1)
								{
									iy = minY;
								}
							}
						}
					}
				}

				if (!hasLava)
				{
					for (int ix = minX; ix < maxX; ++ix)
					{
						double var58 = ((double)(ix + centerX * 16) + 0.5 - x) / offset;

						for (int iz = minZ; iz < maxZ; ++iz)
						{
							double var46 = ((double)(iz + centerZ * 16) + 0.5 - z) / offset;
							int index = (ix * 16 + iz) * 128 + maxY;

							for (int iy = maxY - 1; iy >= minY; --iy)
							{
								double var50 = ((double)iy + 0.5 - y) / offsetY;

								if (var50 > -0.7 && var58 * var58 + var50 * var50 + var46 * var46 < 1.0)
								{
									i8 blockID = pArr[index];

									if (blockID == BLOCK_ID_NETHERRACK || blockID == BLOCK_ID_DIRT || blockID == BLOCK_ID_GRASS)
									{
										pArr[index] = 0;
									}
								}

								--index;
							}
						}
					}

					if (flag)
					{
						break;
					}
				}
			}
		}
	}
}

/** Recursively called by generate() (generate) and optionally by itself. */
void MapGenCavesHell::recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArr)
{
	int caveBatch = m_rand.nextInt(m_rand.nextInt(m_rand.nextInt(10) + 1) + 1);

	if (m_rand.nextInt(5) != 0)
	{
		caveBatch = 0;
	}

	for (int i = 0; i < caveBatch; ++i)
	{
		double tx = (double)(x * 16 + m_rand.nextInt(16));
		double ty = (double)m_rand.nextInt(128);
		double tz = (double)(z * 16 + m_rand.nextInt(16));
		int caveNodeNum = 1;

		if (m_rand.nextInt(4) == 0)
		{
			generateLargeCaveNode(m_rand.nextLong(), centerX, centerZ, pArr, tx, ty, tz);
			caveNodeNum += m_rand.nextInt(4);
		}

		for (int j = 0; j < caveNodeNum; ++j)
		{
			float var17 = m_rand.nextFloat() * (float)Math::PI * 2.0F;
			float var18 = (m_rand.nextFloat() - 0.5F) * 2.0F / 8.0F;
			float n1 = m_rand.nextFloat();
			float n2 = m_rand.nextFloat();
			float var19 = n1 * 2.0F + n2;
			generateCaveNode(m_rand.nextLong(), centerX, centerZ, pArr, tx, ty, tz, var19 * 2.0F, var17, var18, 0, 0, 0.5);
		}
	}
}

void MapGenRavine::generateRavine(i64 seed, int centerX, int centerZ, i8* arr, double x, double y, double z, float par12, float par13, float par14, int begin, int end, double par17)
{
	Random rand(seed);
	double midX = (double)(centerX * 16 + 8);
	double midZ = (double)(centerZ * 16 + 8);
	float var24 = 0.0F;
	float var25 = 0.0F;

	if (end <= 0)
	{
		int var26 = m_range * 16 - 16;
		end = var26 - rand.nextInt(var26 / 4);
	}

	bool flag = false;

	if (begin == -1)
	{
		begin = end / 2;
		flag = true;
	}

	float var27 = 1.0F;

	for (int height = 0; height < 128; ++height)
	{
		if (height == 0 || rand.nextInt(3) == 0)
		{
			float n1 = rand.nextFloat();
			float n2 = rand.nextFloat();
			var27 = 1.0F + n1 * n2 * 1.0F;
		}

		field_75046_d[height] = var27 * var27;
	}

	for (; begin < end; ++begin)
	{
		double offset = 1.5 + (double)(Math::Sin_tab((float)begin * (float)Math::PI / (float)end) * par12 * 1.0F);
		double offsetY = offset * par17;
		offset *= (double)rand.nextFloat() * 0.25 + 0.75;
		offsetY *= (double)rand.nextFloat() * 0.25 + 0.75;
		float beta = Math::Cos_tab(par14);
		float betaY = Math::Sin_tab(par14);
		x += (double)(Math::Cos_tab(par13) * beta);
		y += (double)betaY;
		z += (double)(Math::Sin_tab(par13) * beta);
		par14 *= 0.7F;
		par14 += var25 * 0.05F;
		par13 += var24 * 0.05F;
		var25 *= 0.8F;
		var24 *= 0.5F;
		float n1 = rand.nextFloat();
		float n2 = rand.nextFloat();
		float n3 = rand.nextFloat();
		var25 += (n1 - n2) * n3 * 2.0F;
		n1 = rand.nextFloat();
		n2 = rand.nextFloat();
		n3 = rand.nextFloat();
		var24 += (n1 - n2) * n3 * 4.0F;

		if (flag || rand.nextInt(4) != 0)
		{
			double disX = x - midX;
			double disZ = z - midZ;
			double var38 = (double)(end - begin);
			double var40 = (double)(par12 + 2.0F + 16.0F);

			if (disX * disX + disZ * disZ - var38 * var38 > var40 * var40)
			{
				return;
			}

			if (x >= midX - 16.0 - offset * 2.0 && z >= midZ - 16.0 - offset * 2.0 && x <= midX + 16.0 + offset * 2.0 && z <= midZ + 16.0 + offset * 2.0)
			{
				int minX = int(Math::Floor(x - offset) - centerX * 16 - 1);
				int maxX = int(Math::Floor(x + offset) - centerX * 16 + 1);
				int minY = int(Math::Floor(y - offsetY) - 1);
				int maxY = int(Math::Floor(y + offsetY) + 1);
				int minZ = int(Math::Floor(z - offset) - centerZ * 16 - 1);
				int maxZ = int(Math::Floor(z + offset) - centerZ * 16 + 1);

				if (minX < 0)
				{
					minX = 0;
				}

				if (maxX > 16)
				{
					maxX = 16;
				}

				if (minY < 1)
				{
					minY = 1;
				}

				if (maxY > 120)
				{
					maxY = 120;
				}

				if (minZ < 0)
				{
					minZ = 0;
				}

				if (maxZ > 16)
				{
					maxZ = 16;
				}

				bool hasWater = false;

				for (int ix = minX; !hasWater && ix < maxX; ++ix)
				{
					for (int iz = minZ; !hasWater && iz < maxZ; ++iz)
					{
						for (int iy = maxY + 1; !hasWater && iy >= minY - 1; --iy)
						{
							int index = (ix * 16 + iz) * 128 + iy;

							if (iy >= 0 && iy < 128)
							{
								if (arr[index] == BLOCK_ID_WATERMOVING || arr[index] == BLOCK_ID_WATERSTILL)
								{
									hasWater = true;
								}

								if (iy != minY - 1 && ix != minX && ix != maxX - 1 && iz != minZ && iz != maxZ - 1)
								{
									iy = minY;
								}
							}
						}
					}
				}

				if (!hasWater)
				{
					for (int ix = minX; ix < maxX; ++ix)
					{
						double var59 = ((double)(ix + centerX * 16) + 0.5 - x) / offset;

						for (int iz = minZ; iz < maxZ; ++iz)
						{
							double var45 = ((double)(iz + centerZ * 16) + 0.5 - z) / offset;
							int index = (ix * 16 + iz) * 128 + maxY;
							bool hasGrass = false;

							if (var59 * var59 + var45 * var45 < 1.0)
							{
								for (int iy = maxY - 1; iy >= minY; --iy)
								{
									double var50 = ((double)iy + 0.5 - y) / offsetY;

									double temp1 = (var59 * var59 + var45 * var45) * (double)field_75046_d[iy] + var50 * var50 / 6.0;

									// add by zhouyou
									//  so this is the key C++ different with java. float precision was been zoom in .
									if (temp1 < 1.0)//
									{
										i8 blockID = arr[index];

										if (blockID == BLOCK_ID_GRASS)
										{
											hasGrass = true;
										}

										if (blockID == BLOCK_ID_STONE || blockID == BLOCK_ID_DIRT || blockID == BLOCK_ID_GRASS)
										{
											if (iy < 10)
											{
												arr[index] = (i8)BLOCK_ID_LAVAMOVING;
											}
											else
											{
												arr[index] = 0;

												if (hasGrass && arr[index - 1] == BLOCK_ID_DIRT)
												{
													arr[index - 1] = m_pWorld->getBiomeGenForCoords(ix + centerX * 16, iz + centerZ * 16)->m_topBlock;
												}
											}
										}
									}

									--index;
								}
							}
						}
					}

					if (flag)
					{
						break;
					}
				}
			}
		}
	}
}

void MapGenRavine::recursiveGenerate(World* pWorld, int x, int z, int centerX, int centerZ, i8* pArr)
{
	if (m_rand.nextInt(50) == 0)
	{
		double tx = (double)(x * 16 + m_rand.nextInt(16));
		double ty = (double)(m_rand.nextInt(m_rand.nextInt(40) + 8) + 20);
		double tz = (double)(z * 16 + m_rand.nextInt(16));
		i8 var13 = 1;

		for (int i = 0; i < var13; ++i)
		{
			float r1 = m_rand.nextFloat();
			float r2 = m_rand.nextFloat();
			float r3 = m_rand.nextFloat();
			float var15 = r1 * (float)Math::PI * 2.0F;
			float var16 = (r2 - 0.5F) * 2.0F / 8.0F;
			float var17 = (r3 * 2.0F + m_rand.nextFloat()) * 2.0F;
			generateRavine(m_rand.nextLong(), centerX, centerZ, pArr, tx, ty, tz, var17, var15, var16, 0, 0, 3.0);
		}
	}
}

MapGenNetherBridge::MapGenNetherBridge()
{
	/*spawnList.push_back(new SpawnListEntry(EntityBlaze.class, 10, 2, 3));
	spawnList.push_back(new SpawnListEntry(EntityPigZombie.class, 5, 4, 4));
	spawnList.push_back(new SpawnListEntry(EntitySkeleton.class, 10, 4, 4));
	spawnList.push_back(new SpawnListEntry(EntityMagmaCube.class, 3, 4, 4));*/
}

bool MapGenNetherBridge::canSpawnStructureAtCoords(int x, int z)
{
	int var3 = x >> 4;
	int var4 = z >> 4;
	m_rand.setSeed((i64)(var3 ^ var4 << 4) ^ m_pWorld->getSeed());
	m_rand.nextInt();
	return m_rand.nextInt(3) != 0 ? false : (x != (var3 << 4) + 4 + m_rand.nextInt(8) ? false : z == (var4 << 4) + 4 + m_rand.nextInt(8));
}

StructureStart* MapGenNetherBridge::getStructureStart(int x, int z)
{
	return LordNew StructureNetherBridgeStart(m_pWorld, m_rand, x, z);
}

}
