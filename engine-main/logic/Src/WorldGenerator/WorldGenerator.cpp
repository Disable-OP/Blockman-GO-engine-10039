#include "WorldGenerator.h"
#include "WeightedRandomItem.h"
#include "BiomeGen.h"

#include "Block/BlockManager.h"
#include "Block/Blocks.h"
#include "World/World.h"
#include "World/WorldProvider.h"
#include "TileEntity/TileEntityInventory.h"

namespace BLOCKMAN
{

	WorldGenerator::WorldGenerator()
		: doBlockNotify(false)
	{ }

	WorldGenerator::WorldGenerator(bool par1)
		: doBlockNotify(false)
	{ }

	WorldGenerator::~WorldGenerator()
	{}

	void WorldGenerator::setBlock(World* par1World, int x, int y, int z, int blockID)
	{
		setBlockAndMetadata(par1World, x, y, z, blockID, 0);
	}

	/** Sets the block in the world, notifying neighbors if enabled. */
	void WorldGenerator::setBlockAndMetadata(World* par1World, int x, int y, int z, int blockID, int metadata)
	{
		BlockPos pos(x, y, z);
		if (doBlockNotify)
		{
			par1World->setBlock(pos, blockID, metadata, 3);
		}
		else
		{
			par1World->setBlock(pos, blockID, metadata, 2);
		}
	}

	WorldGenBigMushroom::WorldGenBigMushroom(int mushroomType)
		: WorldGenerator(true)
	{
		this->mushroomType = mushroomType;
	}

	WorldGenBigMushroom::WorldGenBigMushroom()
		: WorldGenerator(false)
	{
		mushroomType = -1;
	}

	bool WorldGenBigMushroom::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int type = rand.nextInt(2);

		if (mushroomType >= 0)
		{
			type = mushroomType;
		}

		int height = rand.nextInt(3) + 4;
		bool flag = true;

		if (y >= 1 && y + height + 1 < 256)
		{

			for (int ty = y; ty <= y + 1 + height; ++ty)
			{
				i8 offsetXZ = 3;

				if (ty <= y + 3)
				{
					offsetXZ = 0;
				}

				for (int tx = x - offsetXZ; tx <= x + offsetXZ && flag; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ && flag; ++tz)
					{
						if (ty >= 0 && ty < 256)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID != 0 && blockID != BLOCK_ID_LEAVES)
							{
								flag = false;
							}
						}
						else
						{
							flag = false;
						}
					}
				}
			}

			if (!flag)
			{
				return false;
			}
			else
			{
				int blockID = pWorld->getBlockId(BlockPos(x, y - 1, z));

				if (blockID != BLOCK_ID_DIRT && blockID != BLOCK_ID_GRASS && blockID != BLOCK_ID_MYCELIUM)
				{
					return false;
				}
				else
				{
					int h = y + height;

					if (type == 1)
					{
						h = y + height - 3;
					}

					for (int ty = h; ty <= y + height; ++ty)
					{
						int offsetXZ = 1;

						if (ty < y + height)
						{
							++offsetXZ;
						}

						if (type == 0)
						{
							offsetXZ = 3;
						}

						for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
						{
							for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
							{
								int metadata = 5;

								if (tx == x - offsetXZ)
								{
									--metadata;
								}

								if (tx == x + offsetXZ)
								{
									++metadata;
								}

								if (tz == z - offsetXZ)
								{
									metadata -= 3;
								}

								if (tz == z + offsetXZ)
								{
									metadata += 3;
								}

								if (type == 0 || ty < y + height)
								{
									if ((tx == x - offsetXZ || tx == x + offsetXZ) && (tz == z - offsetXZ || tz == z + offsetXZ))
									{
										continue;
									}

									if (tx == x - (offsetXZ - 1) && tz == z - offsetXZ)
									{
										metadata = 1;
									}

									if (tx == x - offsetXZ && tz == z - (offsetXZ - 1))
									{
										metadata = 1;
									}

									if (tx == x + (offsetXZ - 1) && tz == z - offsetXZ)
									{
										metadata = 3;
									}

									if (tx == x + offsetXZ && tz == z - (offsetXZ - 1))
									{
										metadata = 3;
									}

									if (tx == x - (offsetXZ - 1) && tz == z + offsetXZ)
									{
										metadata = 7;
									}

									if (tx == x - offsetXZ && tz == z + (offsetXZ - 1))
									{
										metadata = 7;
									}

									if (tx == x + (offsetXZ - 1) && tz == z + offsetXZ)
									{
										metadata = 9;
									}

									if (tx == x + offsetXZ && tz == z + (offsetXZ - 1))
									{
										metadata = 9;
									}
								}

								if (metadata == 5 && ty < y + height)
								{
									metadata = 0;
								}

								if ((metadata != 0 || y >= y + height - 1) && !BlockManager::sOpaqueCubeLookup[pWorld->getBlockId(BlockPos(tx, ty, tz))])
								{
									setBlockAndMetadata(pWorld, tx, ty, tz, BLOCK_ID_MUSHROOMCAP_BROWN + type, metadata);
								}
							}
						}
					}

					for (int iy = 0; iy < height; ++iy)
					{
						int blockID = pWorld->getBlockId(BlockPos(x, y + iy, z));

						if (!BlockManager::sOpaqueCubeLookup[blockID])
						{
							setBlockAndMetadata(pWorld, x, y + iy, z, BLOCK_ID_MUSHROOMCAP_BROWN + type, 10);
						}
					}

					return true;
				}
			}
		}
		else
		{
			return false;
		}
	}

	i8 WorldGenBigTree::otherCoordPairs[6] = { 2, 0, 0, 1, 2, 1 };

	WorldGenBigTree::WorldGenBigTree(bool par1)
		: WorldGenerator(par1)
	{
		worldObj = NULL;
		basePos[0] = 0;
		basePos[1] = 0;
		basePos[2] = 0;
		heightLimit = 0;
		height = 0;
		heightAttenuation = 0.618;
		branchDensity = 1.0;
		branchSlope = 0.381;
		scaleWidth = 1.0;
		leafDensity = 1.0;
		trunkSize = 1;
		heightLimitLimit = 12;
		leafDistanceLimit = 4;
		leafNodes = NULL;
		leafNodePitch = 0;
		//leafNodes;
	}

	WorldGenBigTree::~WorldGenBigTree()
	{
		if (leafNodes)
			LordFree(leafNodes);
	}

	void WorldGenBigTree::generateLeafNodeList()
	{
		height = (int)((double)heightLimit * heightAttenuation);

		if (height >= heightLimit)
		{
			height = heightLimit - 1;
		}

		int leafNum = (int)(1.382 + Math::Pow(leafDensity * (double)heightLimit / 13.0, 2.0));

		if (leafNum < 1)
		{
			leafNum = 1;
		}

		int* pArr = (int*)LordMalloc(sizeof(int) * leafNum * heightLimit * 4);
		memset(pArr, 0, sizeof(int) * leafNum * heightLimit * 4);
		int trunkY = basePos[1] + heightLimit - leafDistanceLimit;// trunk height limit y-axis coordinate
		int pitchIndex = 1;
		int treeY = basePos[1] + height;// tree height y-axis coordinate
		int trunkHeightLimit = trunkY - basePos[1];//trunk height limit
		pArr[0] = basePos[0];
		pArr[1] = trunkY;
		pArr[2] = basePos[2];
		pArr[3] = treeY;
		--trunkY;

		while (trunkHeightLimit >= 0)
		{
			float var8 = layerSize(trunkHeightLimit);

			if (var8 < 0.0F)
			{
				--trunkY;
				--trunkHeightLimit;
			}
			else
			{
				for (int i = 0; i < leafNum; ++i)
				{
					double r = scaleWidth * (double)var8 * ((double)m_Rand.nextFloat() + 0.328);
					double radian = (double)m_Rand.nextFloat() * 2.0 * Math::PI;
					int x = int(Math::Floor(r * Math::Sin(radian) + (double)basePos[0] + 0.5));
					int z = int(Math::Floor(r * Math::Cos(radian) + (double)basePos[2] + 0.5));
					int bottom[3] = { x, trunkY, z };
					int top[3] = { x, trunkY + leafDistanceLimit, z };

					if (checkBlockLine(bottom, top) == -1)
					{
						int var19[3] = { basePos[0], basePos[1], basePos[2] };
						double offsetDistance = Math::Sqrt(Math::Pow((double)Math::Abs(basePos[0] - bottom[0]), 2.0) + Math::Pow((double)Math::Abs(basePos[2] - bottom[2]), 2.0));
						double var22 = offsetDistance * branchSlope;

						if ((double)bottom[1] - var22 > (double)treeY)
						{
							var19[1] = treeY;
						}
						else
						{
							var19[1] = (int)((double)bottom[1] - var22);
						}

						if (checkBlockLine(var19, bottom) == -1)
						{
							pArr[pitchIndex * 4 + 0] = x;
							pArr[pitchIndex * 4 + 1] = trunkY;
							pArr[pitchIndex * 4 + 2] = z;
							pArr[pitchIndex * 4 + 3] = var19[1];
							++pitchIndex;
						}
					}
				}

				--trunkY;
				--trunkHeightLimit;
			}
		}
		leafNodePitch = pitchIndex;
		leafNodes = pArr;  // just use var2 arrays.
	}

	void WorldGenBigTree::genTreeLayer(int x, int y, int z, float size, i8 index, int blockID)
	{
		int offset = (int)((double)size + 0.618);

		//另外两个坐标轴的下标
		i8 otherCoord1 = otherCoordPairs[index];
		i8 otherCoord2 = otherCoordPairs[index + 3];

		int prePosition[3] = { x, y, z };
		int curPosition[3] = { 0, 0, 0 };



		curPosition[index] = prePosition[index];
		for (int offset1 = -offset; offset1 <= offset; ++offset1)
		{
			curPosition[otherCoord1] = prePosition[otherCoord1] + offset1;

			for (int offset2 = -offset; offset2 <= offset; ++offset2)
			{
				double dis = Math::Pow((double)Math::Abs(offset1) + 0.5, 2.0) + Math::Pow((double)Math::Abs(offset2) + 0.5, 2.0);

				if (dis > (double)(size * size))
				{
				}
				else
				{
					curPosition[otherCoord2] = prePosition[otherCoord2] + offset2;
					int id = worldObj->getBlockId(BlockPos(curPosition[0], curPosition[1], curPosition[2]));

					if (id != 0 && id != BLOCK_ID_LEAVES)
					{
					}
					else
					{
						setBlockAndMetadata(worldObj, curPosition[0], curPosition[1], curPosition[2], blockID, 0);
					}
				}
			}
		}
	}

	float WorldGenBigTree::layerSize(int height)
	{
		if ((double)height < (double)((float)heightLimit) * 0.3)
		{
			return -1.618F;
		}
		else
		{
			float var2 = (float)heightLimit / 2.0F;
			float var3 = (float)heightLimit / 2.0F - (float)height;
			float size;

			if (var3 == 0.0F)
			{
				size = var2;
			}
			else if (Math::Abs(var3) >= var2)
			{
				size = 0.0F;
			}
			else
			{
				size = (float)Math::Sqrt(Math::Pow((double)Math::Abs(var2), 2.0) - Math::Pow((double)Math::Abs(var3), 2.0));
			}

			size *= 0.5F;
			return size;
		}
	}

	float WorldGenBigTree::leafSize(int height)
	{
		return height >= 0 && height < leafDistanceLimit ? (height != 0 && height != leafDistanceLimit - 1 ? 3.0F : 2.0F) : -1.0F;
	}

	void WorldGenBigTree::generateLeafNode(int x, int y, int z)
	{
		int leafY = y + leafDistanceLimit;
		for (int ty = y; ty < leafY; ++ty)
		{
			float size = leafSize(ty - y);
			genTreeLayer(x, ty, z, size, 1, BLOCK_ID_LEAVES);
		}
	}

	void WorldGenBigTree::placeBlockLine(int* pBottom, int* pTop, int blockID)
	{
		int differenceValue[3] = { 0, 0, 0 };

		i8 dimension = 0;

		//x,y,z
		for (i8 i = 0; i < 3; ++i)
		{
			differenceValue[i] = pTop[i] - pBottom[i];

			//record dimension
			if (Math::Abs(differenceValue[i]) > Math::Abs(differenceValue[dimension]))
			{
				dimension = i;
			}
		}

		if (differenceValue[dimension] != 0)
		{
			i8 otherCoord1 = otherCoordPairs[dimension];
			i8 otherCoord2 = otherCoordPairs[dimension + 3];
			i8 increment;

			if (differenceValue[dimension] > 0)
			{
				increment = 1;
			}
			else
			{
				increment = -1;
			}


			double slope1 = (double)differenceValue[otherCoord1] / (double)differenceValue[dimension];
			double slope2 = (double)differenceValue[otherCoord2] / (double)differenceValue[dimension];
			int position[3] = { 0, 0, 0 };

			int maxHeight = differenceValue[dimension] + increment;

			for (int height = 0; height != maxHeight; height += increment)
			{
				position[dimension] = int(Math::Floor((double)(pBottom[dimension] + height) + 0.5));
				position[otherCoord1] = int(Math::Floor((double)pBottom[otherCoord1] + (double)height * slope1 + 0.5));
				position[otherCoord2] = int(Math::Floor((double)pBottom[otherCoord2] + (double)height * slope2 + 0.5));

				int disX = Math::Abs(position[0] - pBottom[0]);
				int disZ = Math::Abs(position[2] - pBottom[2]);
				int maxDis = Math::Max(disX, disZ);

				i8 metadata = 0;
				if (maxDis > 0)
				{
					if (disX == maxDis)
					{
						metadata = 4;
					}
					else if (disZ == maxDis)
					{
						metadata = 8;
					}
				}

				setBlockAndMetadata(worldObj, position[0], position[1], position[2], blockID, metadata);
			}
		}
	}

	void WorldGenBigTree::generateLeaves()
	{
		int size = leafNodePitch;

		for (int i = 0; i < size; ++i)
		{
			int x = leafNodes[i * 4 + 0];
			int y = leafNodes[i * 4 + 1];
			int z = leafNodes[i * 4 + 2];
			generateLeafNode(x, y, z);
		}
	}

	void WorldGenBigTree::generateTrunk()
	{
		int x = basePos[0];
		int y = basePos[1];
		int z = basePos[2];
		int ty = basePos[1] + height;
		int bottom[3] = { x, y, z };
		int top[3] = { x, ty, z };
		placeBlockLine(bottom, top, BLOCK_ID_WOOD);

		if (trunkSize == 2)
		{
			++bottom[0];
			++top[0];
			placeBlockLine(bottom, top, BLOCK_ID_WOOD);
			++bottom[2];
			++top[2];
			placeBlockLine(bottom, top, BLOCK_ID_WOOD);
			bottom[0] += -1;
			top[0] += -1;
			placeBlockLine(bottom, top, BLOCK_ID_WOOD);
		}
	}

	void WorldGenBigTree::generateLeafNodeBases()
	{
		int size = leafNodePitch;
		int bottom[3] = { basePos[0], basePos[1], basePos[2] };

		for (int i = 0; i < size; ++i)
		{
			const int* pLeafNode = leafNodes + i * 4;

			int top[3] = { pLeafNode[0], pLeafNode[1], pLeafNode[2] };
			bottom[1] = pLeafNode[3];

			int height = bottom[1] - basePos[1];

			if (leafNodeNeedsBase(height))
			{
				placeBlockLine(bottom, top, BLOCK_ID_WOOD);
			}
		}
	}

	void WorldGenBigTree::generateFinish()
	{
		LordSafeFree(leafNodes);
	}

	int WorldGenBigTree::checkBlockLine(int* pBottom, int* pTop)
	{
		int differenceValue[] = { 0, 0, 0 };
		i8 dimension = 0;

		for (i8 i = 0; i < 3; ++i)
		{
			differenceValue[i] = pTop[i] - pBottom[i];

			if (Math::Abs(differenceValue[i]) > Math::Abs(differenceValue[dimension]))
			{
				dimension = i;
			}
		}

		if (differenceValue[dimension] == 0)
		{
			return -1;
		}
		else
		{
			i8 otherCoord1 = otherCoordPairs[dimension];
			i8 otherCoord2 = otherCoordPairs[dimension + 3];
			i8 increment;

			if (differenceValue[dimension] > 0)
			{
				increment = 1;
			}
			else
			{
				increment = -1;
			}

			double slop1 = (double)differenceValue[otherCoord1] / (double)differenceValue[dimension];
			double slop2 = (double)differenceValue[otherCoord2] / (double)differenceValue[dimension];
			int position[] = { 0, 0, 0 };
			int maxHeight = differenceValue[dimension] + increment;

			int height;
			for (height = 0; height != maxHeight; height += increment)
			{
				position[dimension] = pBottom[dimension] + height;
				position[otherCoord1] = int(Math::Floor((double)pBottom[otherCoord1] + (double)height * slop1));
				position[otherCoord2] = int(Math::Floor((double)pBottom[otherCoord2] + (double)height * slop2));
				int blockID = worldObj->getBlockId(BlockPos(position[0], position[1], position[2]));

				if (blockID != 0 && blockID != BLOCK_ID_LEAVES)
				{
					break;
				}
			}

			return height == maxHeight ? -1 : Math::Abs(height);
		}
	}

	bool WorldGenBigTree::validTreeLocation()
	{
		int bottom[3] = { basePos[0], basePos[1], basePos[2] };
		int top[3] = { basePos[0], basePos[1] + heightLimit - 1, basePos[2] };
		int blockID = worldObj->getBlockId(BlockPos(basePos[0], basePos[1] - 1, basePos[2]));

		if (blockID != 2 && blockID != 3)
		{
			return false;
		}
		else
		{
			int height = checkBlockLine(bottom, top);

			if (height == -1)
			{
				return true;
			}
			else if (height < 6)
			{
				return false;
			}
			else
			{
				heightLimit = height;
				return true;
			}
		}
	}

	void WorldGenBigTree::setScale(double par1, double par3, double par5)
	{
		heightLimitLimit = (int)(par1 * 12.0);

		if (par1 > 0.5)
		{
			leafDistanceLimit = 5;
		}

		scaleWidth = par3;
		leafDensity = par5;
	}

	bool WorldGenBigTree::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		worldObj = pWorld;
		i64 seed = rand.nextLong();
		m_Rand.setSeed(seed);
		basePos[0] = x;
		basePos[1] = y;
		basePos[2] = z;

		if (heightLimit == 0)
		{
			heightLimit = 5 + m_Rand.nextInt(heightLimitLimit);
		}

		if (!validTreeLocation())
		{
			return false;
		}
		else
		{
			generateLeafNodeList();
			generateLeaves();
			generateTrunk();
			generateLeafNodeBases();
			generateFinish();
			return true;
		}
	}

	bool WorldGenCactus::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		for (int i = 0; i < 10; ++i)
		{
			int tx = x + rand.nextInt(8);
			tx -= rand.nextInt(8);
			int ty = y + rand.nextInt(4);
			ty -= rand.nextInt(4);
			int tz = z + rand.nextInt(8);
			tz -= rand.nextInt(8);

			if (pWorld->isAirBlock(BlockPos(tx, ty, tz)))
			{
				int height = 1 + rand.nextInt(rand.nextInt(3) + 1);

				for (int offsetY = 0; offsetY < height; ++offsetY)
				{
					BlockCactus* pcatus = dynamic_cast<BlockCactus*>(BlockManager::sBlocks[BLOCK_ID_CACTUS]);
					if (pcatus->canBlockStay(pWorld, BlockPos(tx, ty + offsetY, tz)))
					{
						pWorld->setBlock(BlockPos(tx, ty + offsetY, tz), BLOCK_ID_CACTUS, 0, 2);
					}
				}
			}
		}

		return true;
	}

	WorldGenClay::WorldGenClay(int numberOfBlocks)
	{
		this->clayBlockId = BLOCK_ID_BLOCKCLAY;
		this->numberOfBlocks = numberOfBlocks;
	}

	bool WorldGenClay::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		if (pWorld->getBlockMaterial(BlockPos(x, y, z)) != BM_Material::BM_MAT_water)
		{
			return false;
		}
		else
		{
			int offsetXZ = rand.nextInt(numberOfBlocks - 2) + 2;
			i8 offsetY = 1;

			for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
			{
				for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
				{
					int disX = tx - x;
					int disZ = tz - z;

					//x^2 + z^2 <= r^2
					if (disX * disX + disZ * disZ <= offsetXZ * offsetXZ)
					{
						for (int ty = y - offsetY; ty <= y + offsetY; ++ty)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID == BLOCK_ID_DIRT || blockID == BLOCK_ID_BLOCKCLAY)
							{
								pWorld->setBlock(BlockPos(tx, ty, tz), clayBlockId, 0, 2);
							}
						}
					}
				}
			}

			return true;
		}
	}

	WorldGenDeadBush::WorldGenDeadBush(int deadBushID)
	{
		this->deadBushID = deadBushID;
	}

	bool WorldGenDeadBush::generate(World* pWorld, Random& rand, int x, int y, int z)
	{

		for (int blockID; ((blockID = pWorld->getBlockId(BlockPos(x, y, z))) == 0 || blockID == BLOCK_ID_LEAVES) && y > 0; --y)
		{
			;
		}

		for (int i = 0; i < 4; ++i)
		{
			int tx = x + rand.nextInt(8);
			tx -= rand.nextInt(8);
			int ty = y + rand.nextInt(4);
			ty -= rand.nextInt(4);
			int tz = z + rand.nextInt(8);
			tz -= rand.nextInt(8);

			BlockDeadBush* pDeadBush = dynamic_cast<BlockDeadBush*>(BlockManager::sBlocks[deadBushID]);
			if (pWorld->isAirBlock(BlockPos(tx, ty, tz)) && pDeadBush->canBlockStay(pWorld, BlockPos(tx, ty, tz)))
			{
				pWorld->setBlock(BlockPos(tx, ty, tz), deadBushID, 0, 2);
			}
		}

		return true;
	}

	bool WorldGenDesertWells::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		while (pWorld->isAirBlock(BlockPos(x, y, z)) && y > 2)
		{
			--y;
		}

		int blockID = pWorld->getBlockId(BlockPos(x, y, z));

		if (blockID != BLOCK_ID_SAND)
		{
			return false;
		}
		else
		{

			for (int offsetX = -2; offsetX <= 2; ++offsetX)
			{
				for (int offsetZ = -2; offsetZ <= 2; ++offsetZ)
				{
					if (pWorld->isAirBlock(BlockPos(x + offsetX, y - 1, z + offsetZ)) && pWorld->isAirBlock(BlockPos(x + offsetX, y - 2, z + offsetZ)))
					{
						return false;
					}
				}
			}

			for (int offsetY = -1; offsetY <= 0; ++offsetY)
			{
				for (int offsetX = -2; offsetX <= 2; ++offsetX)
				{
					for (int offsetZ = -2; offsetZ <= 2; ++offsetZ)
					{
						pWorld->setBlock(BlockPos(x + offsetX, y + offsetY, z + offsetZ), BLOCK_ID_SAND_STONE, 0, 2);
					}
				}
			}

			pWorld->setBlock(BlockPos(x, y, z), BLOCK_ID_WATERMOVING, 0, 2);
			pWorld->setBlock(BlockPos(x - 1, y, z), BLOCK_ID_WATERMOVING, 0, 2);
			pWorld->setBlock(BlockPos(x + 1, y, z), BLOCK_ID_WATERMOVING, 0, 2);
			pWorld->setBlock(BlockPos(x, y, z - 1), BLOCK_ID_WATERMOVING, 0, 2);
			pWorld->setBlock(BlockPos(x, y, z + 1), BLOCK_ID_WATERMOVING, 0, 2);

			for (int offsetX = -2; offsetX <= 2; ++offsetX)
			{
				for (int offsetZ = -2; offsetZ <= 2; ++offsetZ)
				{
					if (offsetX == -2 || offsetX == 2 || offsetZ == -2 || offsetZ == 2)
					{
						pWorld->setBlock(BlockPos(x + offsetX, y + 1, z + offsetZ), BLOCK_ID_SAND_STONE, 0, 2);
					}
				}
			}

			pWorld->setBlock(BlockPos(x + 2, y + 1, z), BLOCK_ID_STONE_SINGLE_SLAB, 1, 2);
			pWorld->setBlock(BlockPos(x - 2, y + 1, z), BLOCK_ID_STONE_SINGLE_SLAB, 1, 2);
			pWorld->setBlock(BlockPos(x, y + 1, z + 2), BLOCK_ID_STONE_SINGLE_SLAB, 1, 2);
			pWorld->setBlock(BlockPos(x, y + 1, z - 2), BLOCK_ID_STONE_SINGLE_SLAB, 1, 2);

			for (int offsetX = -1; offsetX <= 1; ++offsetX)
			{
				for (int offsetZ = -1; offsetZ <= 1; ++offsetZ)
				{
					if (offsetX == 0 && offsetZ == 0)
					{
						pWorld->setBlock(BlockPos(x + offsetX, y + 4, z + offsetZ), BLOCK_ID_SAND_STONE, 0, 2);
					}
					else
					{
						pWorld->setBlock(BlockPos(x + offsetX, y + 4, z + offsetZ), BLOCK_ID_STONE_SINGLE_SLAB, 1, 2);
					}
				}
			}

			for (int offsetY = 1; offsetY <= 3; ++offsetY)
			{
				pWorld->setBlock(BlockPos(x - 1, y + offsetY, z - 1), BLOCK_ID_SAND_STONE, 0, 2);
				pWorld->setBlock(BlockPos(x - 1, y + offsetY, z + 1), BLOCK_ID_SAND_STONE, 0, 2);
				pWorld->setBlock(BlockPos(x + 1, y + offsetY, z - 1), BLOCK_ID_SAND_STONE, 0, 2);
				pWorld->setBlock(BlockPos(x + 1, y + offsetY, z + 1), BLOCK_ID_SAND_STONE, 0, 2);
			}

			return true;
		}
	}

	WeightedRandomChestContent** WorldGenDungeons::field_111189_a = NULL; // = new WeightedRandomChestContent[]{  };
	int WorldGenDungeons::randomChestCount = 0;

	void WorldGenDungeons::initialize()
	{
		field_111189_a = (WeightedRandomChestContent**)LordMalloc(sizeof(WeightedRandomChestContent*) * 128);
		memset(field_111189_a, 0, sizeof(WeightedRandomChestContent*) * 128);
		randomChestCount = 0;
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_SADDLE, 0, 1, 1, 10);		// 73
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_INGOT_IRON, 0, 1, 4, 10);		// 9
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_BREAD, 0, 1, 1, 10);			// 41
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_WHEAT, 0, 1, 4, 10);			// 40
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_GUNPOWDER, 0, 1, 4, 10);		// 33
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_SILK, 0, 1, 4, 10);			// 31
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_BUCKETEMPTY, 0, 1, 1, 10);	// 69
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_APPLEGOLD, 0, 1, 1, 1);		// 66
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_REDSTONE, 0, 1, 4, 10);		// 75
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_RECORD13, 0, 1, 1, 10);		// 2000
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_RECORDCAT, 0, 1, 1, 10);		// 2001
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_FIELD_CI, 0, 1, 1, 10);	// 165
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_FIELD_CF, 0, 1, 1, 2);	// 162
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_FIELD_CE, 0, 1, 1, 5);	// 161
		field_111189_a[randomChestCount++] = LordNew WeightedRandomChestContent(ITEM_ID_FIELD_CG, 0, 1, 1, 1);		// 163
	}

	void WorldGenDungeons::uninitialize()
	{
		WeightedRandomChestContent* content = NULL;
		for (int i = 0; i < 128; ++i)
		{
			content = field_111189_a[i];
			if (!content)
				continue;
			LordDelete(content);
		}
		LordSafeFree(field_111189_a);
	}

	bool WorldGenDungeons::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		i8 height = 3;
		int offsetX = rand.nextInt(2) + 2;
		int offsetZ = rand.nextInt(2) + 2;
		int numberOfAircorner = 0;


		for (int tx = x - offsetX - 1; tx <= x + offsetX + 1; ++tx)
		{
			for (int ty = y - 1; ty <= y + height + 1; ++ty)
			{
				for (int tz = z - offsetZ - 1; tz <= z + offsetZ + 1; ++tz)
				{
					const BM_Material& material = pWorld->getBlockMaterial(BlockPos(tx, ty, tz));

					//bottom is not solid
					if (ty == y - 1 && !material.isSolid())
					{
						return false;
					}

					//top is not solid
					if (ty == y + height + 1 && !material.isSolid())
					{
						return false;
					}

					if ((tx == x - offsetX - 1 || tx == x + offsetX + 1 || tz == z - offsetZ - 1 || tz == z + offsetZ + 1) &&
						ty == y && pWorld->isAirBlock(BlockPos(tx, ty, tz)) && pWorld->isAirBlock(BlockPos(tx, ty + 1, tz)))
					{
						++numberOfAircorner;
					}
				}
			}
		}

		if (numberOfAircorner >= 1 && numberOfAircorner <= 5)
		{
			for (int tx = x - offsetX - 1; tx <= x + offsetX + 1; ++tx)
			{
				for (int ty = y + height; ty >= y - 1; --ty)
				{
					for (int tz = z - offsetZ - 1; tz <= z + offsetZ + 1; ++tz)
					{

						if (tx != x - offsetX - 1 && ty != y - 1 && tz != z - offsetZ - 1 && tx != x + offsetX + 1 && ty != y + height + 1 && tz != z + offsetZ + 1)
						{
							pWorld->setBlockToAir(BlockPos(tx, ty, tz));
						}
						else if (ty >= 0 && !pWorld->getBlockMaterial(BlockPos(tx, ty - 1, tz)).isSolid())
						{
							pWorld->setBlockToAir(BlockPos(tx, ty, tz));
						}
						else if (pWorld->getBlockMaterial(BlockPos(tx, ty, tz)).isSolid())
						{
							if (ty == y - 1 && rand.nextInt(4) != 0)
							{
								pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_COBBLE_STONE_MOSSY, 0, 2);
							}
							else
							{
								pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_COBBLE_STONE, 0, 2);
							}
						}
					}
				}
			}

			int var10 = 0;

			while (var10 < 2)
			{
				int var11 = 0;

				while (true)
				{
					if (var11 < 3)
					{
						int tx = x + rand.nextInt(offsetX * 2 + 1) - offsetX;
						int tz = z + rand.nextInt(offsetZ * 2 + 1) - offsetZ;

						if (pWorld->isAirBlock(BlockPos(tx, y, tz)))
						{
							int numberOfSolid = 0;

							if (pWorld->getBlockMaterial(BlockPos(tx - 1, y, tz)).isSolid())
							{
								++numberOfSolid;
							}

							if (pWorld->getBlockMaterial(BlockPos(tx + 1, y, tz)).isSolid())
							{
								++numberOfSolid;
							}

							if (pWorld->getBlockMaterial(BlockPos(tx, y, tz - 1)).isSolid())
							{
								++numberOfSolid;
							}

							if (pWorld->getBlockMaterial(BlockPos(tx, y, tz + 1)).isSolid())
							{
								++numberOfSolid;
							}

							if (numberOfSolid == 1)
							{
								pWorld->setBlock(BlockPos(tx, y, tz), BLOCK_ID_CHEST, 0, 2);
								simulateCreateChest(pWorld, rand, tx, y, tz);
								/*TileEntityChest var17 = (TileEntityChest)pWorld->getBlockTileEntity(var12, par4, var14);

								if (var17 != null)
								{
									WeightedRandomChestContent.generateChestContents(rand, var16, var17, 8);
								}*/

								goto End1;
							}
						}

						++var11;
						continue;
					}

				End1:

					++var10;
					break;
				}
			}

			pWorld->setBlock(BlockPos(x, y, z), BLOCK_ID_MOB_SPAWNER, 0, 2);


			//add by maxicheng
			int blockId = pWorld->getBlockId(BlockPos(x, y, z));
			if (blockId == BLOCK_ID_MOB_SPAWNER)
			{
				simulateCreateMob(rand);
			}



			/*TileEntityMobSpawner var18 = (TileEntityMobSpawner)pWorld->getBlockTileEntity(par3, par4, par5);

			if (var18 != null)
			{
				var18.getSpawnerLogic().setMobID(this.pickMobSpawner(rand));
			}
			else
			{
				System.err.println("Failed to fetch mob spawner entity at (" + par3 + ", " + par4 + ", " + par5 + ")");
			}*/

			return true;
		}
		return false;
	}

	void WorldGenDungeons::simulateCreateChest(World* pWorld, Random& rand, int x, int y, int z)
	{
		int minlevel[22] = { 1,1,1,1,1, 1, 1, 1, 1,1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
		int maxlevel[22] = { 4,4,4,4,4, 3, 1, 3, 5,5,5, 2, 2, 3, 5, 1, 3, 3, 5, 2, 1, 1 };
		int index = rand.nextInt(22);
		int& a = minlevel[index];
		int& b = maxlevel[index];
		int id = a >= b ? a : rand.nextInt(b - a + 1) + a;

		WeightedRandomChestContent* enchantedBook = LordNew WeightedRandomChestContent(403, 1, 1, 1, 1);
		WeightedRandomChestContent** pArr2 = &enchantedBook;
		WeightedRandomChestContent** contexts = WeightedRandomChestContent::CombineContent(field_111189_a, randomChestCount, pArr2, 1);

		TileEntityChest* chest = LordNew TileEntityChest(TILE_ENTITY_CHEST);

		WeightedRandomChestContent::generateChestContents(rand, contexts, randomChestCount + 1, chest, 8);

		// add by zhouyou
		// juset simulate create some itemStack in chest. for keep consistent for random seeds.
		LordSafeDelete(chest);
		LordSafeDelete(enchantedBook);
		LordSafeFree(contexts);
	}

	void WorldGenDungeons::simulateCreateMob(Random& rand)
	{
		rand.next(4);
	}

	bool WorldGenFire::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		for (int i = 0; i < 64; ++i)
		{
			int tx = x + rand.nextInt(8);
			tx -= rand.nextInt(8);
			int ty = y + rand.nextInt(4);
			ty -= rand.nextInt(4);
			int tz = z + rand.nextInt(8);
			tz -= rand.nextInt(8);

			if (pWorld->isAirBlock(BlockPos(tx, ty, tz)) && pWorld->getBlockId(BlockPos(tx, ty - 1, tz)) == BLOCK_ID_NETHERRACK)
			{
				pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_FIRE, 0, 2);
			}
		}

		return true;
	}

	WorldGenFlowers::WorldGenFlowers(int plantBlockId)
	{
		this->plantBlockId = plantBlockId;
	}

	bool WorldGenFlowers::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		for (int i = 0; i < 64; ++i)
		{
			int tx = x + rand.nextInt(8);
			tx -= rand.nextInt(8);
			int ty = y + rand.nextInt(4);
			ty -= rand.nextInt(4);
			int tz = z + rand.nextInt(8);
			tz -= rand.nextInt(8);

			Block* pBlock = BlockManager::sBlocks[plantBlockId];
			if (pWorld->isAirBlock(BlockPos(tx, ty, tz)) &&
				(!pWorld->m_provider->hasNoSky || ty < 127) &&
				pBlock->canBlockStay(pWorld, BlockPos(tx, ty, tz)))
			{
				pWorld->setBlock(BlockPos(tx, ty, tz), plantBlockId, 0, 2);
			}
		}

		return true;
	}

	WorldGenForest::WorldGenForest(bool par1)
		: WorldGenerator(par1)
	{
	}

	bool WorldGenForest::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int height = rand.nextInt(3) + 5;
		bool flag = true;

		if (y >= 1 && y + height + 1 <= 256)
		{

			for (int ty = y; ty <= y + 1 + height; ++ty)
			{
				i8 offsetXZ = 1;

				if (ty == y)
				{
					offsetXZ = 0;
				}

				if (ty >= y + 1 + height - 2)
				{
					offsetXZ = 2;
				}

				for (int tx = x - offsetXZ; tx <= x + offsetXZ && flag; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ && flag; ++tz)
					{
						if (ty >= 0 && ty < 256)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID != 0 && blockID != BLOCK_ID_LEAVES)
							{
								flag = false;
							}
						}
						else
						{
							flag = false;
						}
					}
				}
			}

			if (!flag)
			{
				return false;
			}
			else
			{
				int blockID = pWorld->getBlockId(BlockPos(x, y - 1, z));

				if ((blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_DIRT) && y < 256 - height - 1)
				{
					setBlock(pWorld, x, y - 1, z, BLOCK_ID_DIRT);

					//generate leaf
					for (int ty = y - 3 + height; ty <= y + height; ++ty)
					{
						int curHeight = ty - (y + height);
						int offsetXZ = 1 - curHeight / 2;

						for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
						{
							int disX = tx - x;

							for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
							{
								int disZ = tz - z;

								if (Math::Abs(disX) != offsetXZ || Math::Abs(disZ) != offsetXZ || rand.nextInt(2) != 0 && curHeight != 0)
								{
									int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

									if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
									{
										setBlockAndMetadata(pWorld, tx, ty, tz, BLOCK_ID_LEAVES, 2);
									}
								}
							}
						}
					}

					//generate wood
					for (int offsetY = 0; offsetY < height; ++offsetY)
					{
						int blockID = pWorld->getBlockId(BlockPos(x, y + offsetY, z));

						if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
						{
							setBlockAndMetadata(pWorld, x, y + offsetY, z, BLOCK_ID_WOOD, 2);
						}
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	bool WorldGenGlowStone1::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		if (!pWorld->isAirBlock(BlockPos(x, y, z)))
		{
			return false;
		}
		else if (pWorld->getBlockId(BlockPos(x, y + 1, z)) != BLOCK_ID_NETHERRACK)
		{
			return false;
		}
		else
		{
			pWorld->setBlock(BlockPos(x, y, z), BLOCK_ID_GLOW_STONE, 0, 2);

			for (int i = 0; i < 1500; ++i)
			{
				int tx = x + rand.nextInt(8);
				tx -= rand.nextInt(8);
				int ty = y - rand.nextInt(12);
				int tz = z + rand.nextInt(8);
				tz -= rand.nextInt(8);

				if (pWorld->getBlockId(BlockPos(tx, ty, tz)) == 0)
				{
					int numberOfGlowstone = 0;

					for (int j = 0; j < 6; ++j)
					{
						int blockID = 0;

						if (j == 0)
						{
							blockID = pWorld->getBlockId(BlockPos(tx - 1, ty, tz));
						}

						if (j == 1)
						{
							blockID = pWorld->getBlockId(BlockPos(tx + 1, ty, tz));
						}

						if (j == 2)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty - 1, tz));
						}

						if (j == 3)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty + 1, tz));
						}

						if (j == 4)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty, tz - 1));
						}

						if (j == 5)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty, tz + 1));
						}

						if (blockID == BLOCK_ID_GLOW_STONE)
						{
							++numberOfGlowstone;
						}
					}

					if (numberOfGlowstone == 1)
					{
						pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_GLOW_STONE, 0, 2);
					}
				}
			}

			return true;
		}
	}

	bool WorldGenGlowStone2::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		if (!pWorld->isAirBlock(BlockPos(x, y, z)))
		{
			return false;
		}
		else if (pWorld->getBlockId(BlockPos(x, y + 1, z)) != BLOCK_ID_NETHERRACK)
		{
			return false;
		}
		else
		{
			pWorld->setBlock(BlockPos(x, y, z), BLOCK_ID_GLOW_STONE, 0, 2);

			for (int i = 0; i < 1500; ++i)
			{
				int tx = x + rand.nextInt(8);
				tx -= rand.nextInt(8);
				int ty = y - rand.nextInt(12);
				int tz = z + rand.nextInt(8);
				tz -= rand.nextInt(8);

				if (pWorld->getBlockId(BlockPos(tx, ty, tz)) == 0)
				{
					int numberOfGlowstone = 0;

					for (int j = 0; j < 6; ++j)
					{
						int blockID = 0;

						if (j == 0)
						{
							blockID = pWorld->getBlockId(BlockPos(tx - 1, ty, tz));
						}

						if (j == 1)
						{
							blockID = pWorld->getBlockId(BlockPos(tx + 1, ty, tz));
						}

						if (j == 2)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty - 1, tz));
						}

						if (j == 3)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty + 1, tz));
						}

						if (j == 4)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty, tz - 1));
						}

						if (j == 5)
						{
							blockID = pWorld->getBlockId(BlockPos(tx, ty, tz + 1));
						}

						if (blockID == BLOCK_ID_GLOW_STONE)
						{
							++numberOfGlowstone;
						}
					}

					if (numberOfGlowstone == 1)
					{
						pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_GLOW_STONE, 0, 2);
					}
				}
			}

			return true;
		}
	}

	WorldGenHellLava::WorldGenHellLava(int par1, bool par2)
	{
		hellLavaID = par1;
		field_94524_b = par2;
	}

	bool WorldGenHellLava::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		if (pWorld->getBlockId(BlockPos(x, y + 1, z)) != BLOCK_ID_NETHERRACK)
		{
			return false;
		}
		else if (pWorld->getBlockId(BlockPos(x, y, z)) != 0 && pWorld->getBlockId(BlockPos(x, y, z)) != BLOCK_ID_NETHERRACK)
		{
			return false;
		}
		else
		{
			int numberOfNetherrack = 0;

			if (pWorld->getBlockId(BlockPos(x - 1, y, z)) == BLOCK_ID_NETHERRACK)
			{
				++numberOfNetherrack;
			}

			if (pWorld->getBlockId(BlockPos(x + 1, y, z)) == BLOCK_ID_NETHERRACK)
			{
				++numberOfNetherrack;
			}

			if (pWorld->getBlockId(BlockPos(x, y, z - 1)) == BLOCK_ID_NETHERRACK)
			{
				++numberOfNetherrack;
			}

			if (pWorld->getBlockId(BlockPos(x, y, z + 1)) == BLOCK_ID_NETHERRACK)
			{
				++numberOfNetherrack;
			}

			if (pWorld->getBlockId(BlockPos(x, y - 1, z)) == BLOCK_ID_NETHERRACK)
			{
				++numberOfNetherrack;
			}

			int numberOfAir = 0;

			if (pWorld->isAirBlock(BlockPos(x - 1, y, z)))
			{
				++numberOfAir;
			}

			if (pWorld->isAirBlock(BlockPos(x + 1, y, z)))
			{
				++numberOfAir;
			}

			if (pWorld->isAirBlock(BlockPos(x, y, z - 1)))
			{
				++numberOfAir;
			}

			if (pWorld->isAirBlock(BlockPos(x, y, z + 1)))
			{
				++numberOfAir;
			}

			if (pWorld->isAirBlock(BlockPos(x, y - 1, z)))
			{
				++numberOfAir;
			}

			if (!field_94524_b && numberOfNetherrack == 4 && numberOfAir == 1 || numberOfNetherrack == 5)
			{
				pWorld->setBlock(BlockPos(x, y, z), hellLavaID, 0, 2);
				pWorld->m_scheduledUpdatesAreImmediate = true;
				BlockManager::sBlocks[hellLavaID]->updateTick(pWorld, BlockPos(x, y, z), rand);
				pWorld->m_scheduledUpdatesAreImmediate = false;
			}

			return true;
		}
	}

	WorldGenHugeTrees::WorldGenHugeTrees(bool par1, int baseHeight, int woodMetadata, int leavesMetadata)
		: WorldGenerator(par1)
	{
		this->baseHeight = baseHeight;
		this->woodMetadata = woodMetadata;
		this->leavesMetadata = leavesMetadata;
	}

	void WorldGenHugeTrees::setParam(int baseHeight, int woodMetadata, int leavesMetadata)
	{
		this->baseHeight = baseHeight;
		this->woodMetadata = woodMetadata;
		this->leavesMetadata = leavesMetadata;
	}

	bool WorldGenHugeTrees::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int height = rand.nextInt(3) + baseHeight;
		bool flag = true;

		if (y >= 1 && y + height + 1 <= 256)
		{

			for (int ty = y; ty <= y + 1 + height; ++ty)
			{
				i8 offsetXZ = 2;

				if (ty == y)
				{
					offsetXZ = 1;
				}

				if (ty >= y + 1 + height - 2)
				{
					offsetXZ = 2;
				}

				for (int tx = x - offsetXZ; tx <= x + offsetXZ && flag; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ && flag; ++tz)
					{
						if (ty >= 0 && ty < 256)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID != 0 && blockID != BLOCK_ID_LEAVES &&
								blockID != BLOCK_ID_GRASS &&
								blockID != BLOCK_ID_DIRT &&
								blockID != BLOCK_ID_WOOD &&
								blockID != BLOCK_ID_WOOD2 &&
								blockID != BLOCK_ID_SAPLING)
							{
								flag = false;
							}
						}
						else
						{
							flag = false;
						}
					}
				}
			}

			if (!flag)
			{
				return false;
			}
			else
			{
				int blockID = pWorld->getBlockId(BlockPos(x, y - 1, z));

				if ((blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_DIRT) && y < 256 - height - 1)
				{
					pWorld->setBlock(BlockPos(x, y - 1, z), BLOCK_ID_DIRT, 0, 2);
					pWorld->setBlock(BlockPos(x + 1, y - 1, z), BLOCK_ID_DIRT, 0, 2);
					pWorld->setBlock(BlockPos(x, y - 1, z + 1), BLOCK_ID_DIRT, 0, 2);
					pWorld->setBlock(BlockPos(x + 1, y - 1, z + 1), BLOCK_ID_DIRT, 0, 2);
					growLeaves(pWorld, x, z, y + height, 2, rand);

					for (int ty = y + height - 2 - rand.nextInt(4); ty > y + height / 2; ty -= 2 + rand.nextInt(4))
					{
						float radian = rand.nextFloat() * (float)Math::PI * 2.0F;
						int tx = x + (int)(0.5F + Math::Cos_tab(radian) * 4.0F);
						int tz = z + (int)(0.5F + Math::Sin_tab(radian) * 4.0F);
						growLeaves(pWorld, tx, tz, ty, 0, rand);

						for (int r = 0; r < 5; ++r)
						{
							tx = x + (int)(1.5F + Math::Cos_tab(radian) * (float)r);
							tz = z + (int)(1.5F + Math::Sin_tab(radian) * (float)r);
							setBlockAndMetadata(pWorld, tx, ty - 3 + r / 2, tz, BLOCK_ID_WOOD, woodMetadata);
						}
					}

					for (int offsetY = 0; offsetY < height; ++offsetY)
					{
						int blockID = pWorld->getBlockId(BlockPos(x, y + offsetY, z));

						if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
						{
							setBlockAndMetadata(pWorld, x, y + offsetY, z, BLOCK_ID_WOOD, woodMetadata);

							if (offsetY > 0)
							{
								if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x - 1, y + offsetY, z)))
								{
									setBlockAndMetadata(pWorld, x - 1, y + offsetY, z, BLOCK_ID_VINE, 8);
								}

								if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x, y + offsetY, z - 1)))
								{
									setBlockAndMetadata(pWorld, x, y + offsetY, z - 1, BLOCK_ID_VINE, 1);
								}
							}
						}

						if (offsetY < height - 1)
						{
							blockID = pWorld->getBlockId(BlockPos(x + 1, y + offsetY, z));

							if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
							{
								setBlockAndMetadata(pWorld, x + 1, y + offsetY, z, BLOCK_ID_WOOD, woodMetadata);

								if (offsetY > 0)
								{
									if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x + 2, y + offsetY, z)))
									{
										setBlockAndMetadata(pWorld, x + 2, y + offsetY, z, BLOCK_ID_VINE, 2);
									}

									if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x + 1, y + offsetY, z - 1)))
									{
										setBlockAndMetadata(pWorld, x + 1, y + offsetY, z - 1, BLOCK_ID_VINE, 1);
									}
								}
							}

							blockID = pWorld->getBlockId(BlockPos(x + 1, y + offsetY, z + 1));

							if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
							{
								setBlockAndMetadata(pWorld, x + 1, y + offsetY, z + 1, BLOCK_ID_WOOD, woodMetadata);

								if (offsetY > 0)
								{
									if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x + 2, y + offsetY, z + 1)))
									{
										setBlockAndMetadata(pWorld, x + 2, y + offsetY, z + 1, BLOCK_ID_VINE, 2);
									}

									if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x + 1, y + offsetY, z + 2)))
									{
										setBlockAndMetadata(pWorld, x + 1, y + offsetY, z + 2, BLOCK_ID_VINE, 4);
									}
								}
							}

							blockID = pWorld->getBlockId(BlockPos(x, y + offsetY, z + 1));

							if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
							{
								setBlockAndMetadata(pWorld, x, y + offsetY, z + 1, BLOCK_ID_WOOD, woodMetadata);

								if (offsetY > 0)
								{
									if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x - 1, y + offsetY, z + 1)))
									{
										setBlockAndMetadata(pWorld, x - 1, y + offsetY, z + 1, BLOCK_ID_VINE, 8);
									}

									if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x, y + offsetY, z + 2)))
									{
										setBlockAndMetadata(pWorld, x, y + offsetY, z + 2, BLOCK_ID_VINE, 4);
									}
								}
							}
						}
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	void WorldGenHugeTrees::growLeaves(World* pWorld, int x, int z, int y, int par5, Random& rand)
	{
		i8 height = 2;

		for (int ty = y - height; ty <= y; ++ty)
		{
			int disY = ty - y;
			int offsetXZ = par5 + 1 - disY;

			for (int tx = x - offsetXZ; tx <= x + offsetXZ + 1; ++tx)
			{
				int disX = tx - x;

				for (int tz = z - offsetXZ; tz <= z + offsetXZ + 1; ++tz)
				{
					int disZ = tz - z;

					if ((disX >= 0 || disZ >= 0 || disX * disX + disZ * disZ <= offsetXZ * offsetXZ) &&
						(disX <= 0 && disZ <= 0 || disX * disX + disZ * disZ <= (offsetXZ + 1) * (offsetXZ + 1)) &&
						(rand.nextInt(4) != 0 || disX * disX + disZ * disZ <= (offsetXZ - 1) * (offsetXZ - 1)))
					{
						int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

						if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
						{
							setBlockAndMetadata(pWorld, tx, ty, tz, BLOCK_ID_LEAVES, leavesMetadata);
						}
					}
				}
			}
		}
	}

	WorldGenLakes::WorldGenLakes(int blockIndex)
	{
		this->blockIndex = blockIndex;
	}

	bool WorldGenLakes::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		x -= 8;

		for (z -= 8; y > 5 && pWorld->isAirBlock(BlockPos(x, y, z)); --y)
		{
			;
		}

		if (y <= 4)
		{
			return false;
		}
		else
		{
			y -= 4;
			bool* isAir = (bool*)LordMalloc(sizeof(bool) * 2048);
			memset(isAir, 0, sizeof(bool) * 2048);

			int var7 = rand.nextInt(4) + 4;
			int i;

			for (i = 0; i < var7; ++i)
			{
				double var9 = rand.nextDouble() * 6.0 + 3.0;
				double var11 = rand.nextDouble() * 4.0 + 2.0;
				double var13 = rand.nextDouble() * 6.0 + 3.0;
				double var15 = rand.nextDouble() * (16.0 - var9 - 2.0) + 1.0 + var9 / 2.0;
				double var17 = rand.nextDouble() * (8.0 - var11 - 4.0) + 2.0 + var11 / 2.0;
				double var19 = rand.nextDouble() * (16.0 - var13 - 2.0) + 1.0 + var13 / 2.0;

				for (int ix = 1; ix < 15; ++ix)
				{
					for (int iz = 1; iz < 15; ++iz)
					{
						for (int iy = 1; iy < 7; ++iy)
						{
							double var24 = ((double)ix - var15) / (var9 / 2.0);
							double var26 = ((double)iy - var17) / (var11 / 2.0);
							double var28 = ((double)iz - var19) / (var13 / 2.0);
							double var30 = var24 * var24 + var26 * var26 + var28 * var28;

							if (var30 < 1.0)
							{
								isAir[(ix * 16 + iz) * 8 + iy] = true;
							}
						}
					}
				}
			}


			bool isGround;
			//check
			for (int offsetX = 0; offsetX < 16; ++offsetX)
			{
				for (int offsetZ = 0; offsetZ < 16; ++offsetZ)
				{
					for (int offsetY = 0; offsetY < 8; ++offsetY)
					{
						isGround = !isAir[(offsetX * 16 + offsetZ) * 8 + offsetY] &&
							(offsetX < 15 &&
								isAir[((offsetX + 1) * 16 + offsetZ) * 8 + offsetY] || offsetX > 0 &&
								isAir[((offsetX - 1) * 16 + offsetZ) * 8 + offsetY] || offsetZ < 15 &&
								isAir[(offsetX * 16 + offsetZ + 1) * 8 + offsetY] || offsetZ > 0 &&
								isAir[(offsetX * 16 + (offsetZ - 1)) * 8 + offsetY] || offsetY < 7 &&
								isAir[(offsetX * 16 + offsetZ) * 8 + offsetY + 1] || offsetY > 0 &&
								isAir[(offsetX * 16 + offsetZ) * 8 + (offsetY - 1)]);

						if (isGround)
						{
							const BM_Material& material = pWorld->getBlockMaterial(BlockPos(x + offsetX, y + offsetY, z + offsetZ));

							if (offsetY >= 4 && material.isLiquid())
							{
								LordSafeFree(isAir);
								return false;
							}

							if (offsetY < 4 && !material.isSolid() && pWorld->getBlockId(BlockPos(x + offsetX, y + offsetY, z + offsetZ)) != blockIndex)
							{
								LordSafeFree(isAir);
								return false;
							}
						}
					}
				}
			}

			//generate lake
			for (int offsetX = 0; offsetX < 16; ++offsetX)
			{
				for (int offsetZ = 0; offsetZ < 16; ++offsetZ)
				{
					for (int offsetY = 0; offsetY < 8; ++offsetY)
					{
						if (isAir[(offsetX * 16 + offsetZ) * 8 + offsetY])
						{
							pWorld->setBlock(BlockPos(x + offsetX, y + offsetY, z + offsetZ), offsetY >= 4 ? 0 : blockIndex, 0, 2);
						}
					}
				}
			}

			for (int offsetX = 0; offsetX < 16; ++offsetX)
			{
				for (int offsetZ = 0; offsetZ < 16; ++offsetZ)
				{
					for (int offsetY = 4; offsetY < 8; ++offsetY)
					{
						if (isAir[(offsetX * 16 + offsetZ) * 8 + offsetY] &&
							pWorld->getBlockId(BlockPos(x + offsetX, y + offsetY - 1, z + offsetZ)) == BLOCK_ID_DIRT)
							// todo.
							//pWorld->getSavedLightValue(SKY_LIGHT_VALUE, BlockPos(x + offsetX, y + offsetY, z + offsetZ)) > 0)
						{
							BiomeGenBase* pBiome = pWorld->getBiomeGenForCoords(x + offsetX, z + offsetZ);

							if (pBiome->m_topBlock == BLOCK_ID_MYCELIUM)
							{
								pWorld->setBlock(BlockPos(x + offsetX, y + offsetY - 1, z + offsetZ), BLOCK_ID_MYCELIUM, 0, 2);
							}
							else
							{
								pWorld->setBlock(BlockPos(x + offsetX, y + offsetY - 1, z + offsetZ), BLOCK_ID_GRASS, 0, 2);
							}
						}
					}
				}
			}

			if (BlockManager::sBlocks[blockIndex]->getMaterial() == BM_Material::BM_MAT_lava)
			{
				for (int offsetX = 0; offsetX < 16; ++offsetX)
				{
					for (int offsetZ = 0; offsetZ < 16; ++offsetZ)
					{
						for (int offsetY = 0; offsetY < 8; ++offsetY)
						{
							isGround = !isAir[(offsetX * 16 + offsetZ) * 8 + offsetY] &&
								(offsetX < 15 &&
									isAir[((offsetX + 1) * 16 + offsetZ) * 8 + offsetY] ||
									offsetX > 0 && isAir[((offsetX - 1) * 16 + offsetZ) * 8 + offsetY] ||
									offsetZ < 15 && isAir[(offsetX * 16 + offsetZ + 1) * 8 + offsetY] ||
									offsetZ > 0 && isAir[(offsetX * 16 + (offsetZ - 1)) * 8 + offsetY] ||
									offsetY < 7 && isAir[(offsetX * 16 + offsetZ) * 8 + offsetY + 1] ||
									offsetY > 0 && isAir[(offsetX * 16 + offsetZ) * 8 + (offsetY - 1)]);

							if (isGround && (offsetY < 4 || rand.nextInt(2) != 0) && pWorld->getBlockMaterial(BlockPos(x + offsetX, y + offsetY, z + offsetZ)).isSolid())
							{
								pWorld->setBlock(BlockPos(x + offsetX, y + offsetY, z + offsetZ), BLOCK_ID_STONE, 0, 2);
							}
						}
					}
				}
			}

			if (BlockManager::sBlocks[blockIndex]->getMaterial() == BM_Material::BM_MAT_water)
			{
				for (int offsetX = 0; offsetX < 16; ++offsetX)
				{
					for (int offsetZ = 0; offsetZ < 16; ++offsetZ)
					{

						if (pWorld->isBlockFreezable(BlockPos(x + offsetX, y + 4, z + offsetZ)))
						{
							pWorld->setBlock(BlockPos(x + offsetX, y + 4, z + offsetZ), BLOCK_ID_ICE, 0, 2);
						}
					}
				}
			}

			LordSafeFree(isAir);

			return true;
		}
	}

	WorldGenLiquids::WorldGenLiquids(int liquidBlockId)
	{
		this->liquidBlockId = liquidBlockId;
	}

	bool WorldGenLiquids::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		if (pWorld->getBlockId(BlockPos(x, y + 1, z)) != BLOCK_ID_STONE)
		{
			return false;
		}
		else if (pWorld->getBlockId(BlockPos(x, y - 1, z)) != BLOCK_ID_STONE)
		{
			return false;
		}
		else if (pWorld->getBlockId(BlockPos(x, y, z)) != 0 && pWorld->getBlockId(BlockPos(x, y, z)) != BLOCK_ID_STONE)
		{
			return false;
		}
		else
		{
			int numberOfStone = 0;

			if (pWorld->getBlockId(BlockPos(x - 1, y, z)) == BLOCK_ID_STONE)
			{
				++numberOfStone;
			}

			if (pWorld->getBlockId(BlockPos(x + 1, y, z)) == BLOCK_ID_STONE)
			{
				++numberOfStone;
			}

			if (pWorld->getBlockId(BlockPos(x, y, z - 1)) == BLOCK_ID_STONE)
			{
				++numberOfStone;
			}

			if (pWorld->getBlockId(BlockPos(x, y, z + 1)) == BLOCK_ID_STONE)
			{
				++numberOfStone;
			}

			int numberOfAir = 0;

			if (pWorld->isAirBlock(BlockPos(x - 1, y, z)))
			{
				++numberOfAir;
			}

			if (pWorld->isAirBlock(BlockPos(x + 1, y, z)))
			{
				++numberOfAir;
			}

			if (pWorld->isAirBlock(BlockPos(x, y, z - 1)))
			{
				++numberOfAir;
			}

			if (pWorld->isAirBlock(BlockPos(x, y, z + 1)))
			{
				++numberOfAir;
			}

			if (numberOfStone == 3 && numberOfAir == 1)
			{
				pWorld->setBlock(BlockPos(x, y, z), liquidBlockId, 0, 2);
				pWorld->m_scheduledUpdatesAreImmediate = true;
				BlockManager::sBlocks[liquidBlockId]->updateTick(pWorld, BlockPos(x, y, z), rand);
				pWorld->m_scheduledUpdatesAreImmediate = false;
			}

			return true;
		}
	}

	WorldGenMinable::WorldGenMinable(int minableBlockId, int numberOfBlocks)
	{
		this->minableBlockId = minableBlockId;
		this->numberOfBlocks = numberOfBlocks;
		this->replaceId = BLOCK_ID_STONE;
	}

	WorldGenMinable::WorldGenMinable(int minableBlockId, int numberOfBlocks, int replaceId)
	{
		this->minableBlockId = minableBlockId;
		this->numberOfBlocks = numberOfBlocks;
		this->replaceId = replaceId;
	}

	bool WorldGenMinable::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		float var6 = rand.nextFloat() * (float)Math::PI;
		double var7 = (double)((float)(x + 8) + Math::Sin_tab(var6) * (float)numberOfBlocks / 8.0F);
		double var9 = (double)((float)(x + 8) - Math::Sin_tab(var6) * (float)numberOfBlocks / 8.0F);
		double var11 = (double)((float)(z + 8) + Math::Cos_tab(var6) * (float)numberOfBlocks / 8.0F);
		double var13 = (double)((float)(z + 8) - Math::Cos_tab(var6) * (float)numberOfBlocks / 8.0F);
		double var15 = (double)(y + rand.nextInt(3) - 2);
		double var17 = (double)(y + rand.nextInt(3) - 2);

		for (int i = 0; i <= numberOfBlocks; ++i)
		{
			double var20 = var7 + (var9 - var7) * (double)i / (double)numberOfBlocks;
			double var22 = var15 + (var17 - var15) * (double)i / (double)numberOfBlocks;
			double var24 = var11 + (var13 - var11) * (double)i / (double)numberOfBlocks;
			double var26 = rand.nextDouble() * (double)numberOfBlocks / 16.0;
			double var28 = (double)(Math::Sin_tab((float)i * (float)Math::PI / (float)numberOfBlocks) + 1.0F) * var26 + 1.0;
			double var30 = (double)(Math::Sin_tab((float)i * (float)Math::PI / (float)numberOfBlocks) + 1.0F) * var26 + 1.0;
			int var32 = int(Math::Floor(var20 - var28 / 2.0));
			int var33 = int(Math::Floor(var22 - var30 / 2.0));
			int var34 = int(Math::Floor(var24 - var28 / 2.0));
			int var35 = int(Math::Floor(var20 + var28 / 2.0));
			int var36 = int(Math::Floor(var22 + var30 / 2.0));
			int var37 = int(Math::Floor(var24 + var28 / 2.0));

			for (int tx = var32; tx <= var35; ++tx)
			{
				double var39 = ((double)tx + 0.5 - var20) / (var28 / 2.0);

				if (var39 * var39 < 1.0)
				{
					for (int ty = var33; ty <= var36; ++ty)
					{
						double var42 = ((double)ty + 0.5 - var22) / (var30 / 2.0);

						if (var39 * var39 + var42 * var42 < 1.0)
						{
							for (int tz = var34; tz <= var37; ++tz)
							{
								double var45 = ((double)tz + 0.5 - var24) / (var28 / 2.0);

								if (var39 * var39 + var42 * var42 + var45 * var45 < 1.0 && pWorld->getBlockId(BlockPos(tx, ty, tz)) == replaceId)
								{
									pWorld->setBlock(BlockPos(tx, ty, tz), minableBlockId, 0, 2);
								}
							}
						}
					}
				}
			}
		}

		return true;
	}

	bool WorldGenPumpkin::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		for (int i = 0; i < 64; ++i)
		{
			int tx = x + rand.nextInt(8);
			tx -= rand.nextInt(8);
			int ty = y + rand.nextInt(4);
			ty -= rand.nextInt(4);
			int tz = z + rand.nextInt(8);
			tz -= rand.nextInt(8);

			if (pWorld->isAirBlock(BlockPos(tx, ty, tz)) &&
				pWorld->getBlockId(BlockPos(tx, ty - 1, tz)) == BLOCK_ID_GRASS /*&&
				Block.pumpkin.canPlaceBlockAt(pWorld, var7, var8, var9)*/)
			{
				pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_PUMPKIN, rand.nextInt(4), 2);
			}
		}

		return true;
	}

	bool WorldGenReed::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		for (int i = 0; i < 20; ++i)
		{
			int tx = x + rand.nextInt(4);
			tx -= rand.nextInt(4);
			int ty = y;
			int tz = z + rand.nextInt(4);
			tz -= rand.nextInt(4);

			if (pWorld->isAirBlock(BlockPos(tx, y, tz)) &&
				(pWorld->getBlockMaterial(BlockPos(tx - 1, y - 1, tz)) == BM_Material::BM_MAT_water ||
					pWorld->getBlockMaterial(BlockPos(tx + 1, y - 1, tz)) == BM_Material::BM_MAT_water ||
					pWorld->getBlockMaterial(BlockPos(tx, y - 1, tz - 1)) == BM_Material::BM_MAT_water ||
					pWorld->getBlockMaterial(BlockPos(tx, y - 1, tz + 1)) == BM_Material::BM_MAT_water))
			{
				int height = 2 + rand.nextInt(rand.nextInt(3) + 1);

				for (int offsetY = 0; offsetY < height; ++offsetY)
				{
					//modify by maxicheng
					if (BlockManager::reed->canBlockStay(pWorld, BlockPos(tx, ty + offsetY, tz)))
					{
						pWorld->setBlock(BlockPos(tx, ty + offsetY, tz), BLOCK_ID_REED, 0, 2);
					}
				}
			}
		}

		return true;
	}

	WorldGenSand::WorldGenSand(int radius, int sandID)
	{
		this->radius = radius;
		this->sandID = sandID;
	}

	bool WorldGenSand::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		if (pWorld->getBlockMaterial(BlockPos(x, y, z)) != BM_Material::BM_MAT_water)
		{
			return false;
		}
		else
		{
			int offsetXZ = rand.nextInt(radius - 2) + 2;
			i8 offsetY = 2;

			for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
			{
				for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
				{
					int disX = tx - x;
					int disZ = tz - z;

					if (disX * disX + disZ * disZ <= offsetXZ * offsetXZ)
					{
						for (int ty = y - offsetY; ty <= y + offsetY; ++ty)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID == BLOCK_ID_DIRT || blockID == BLOCK_ID_GRASS)
							{
								pWorld->setBlock(BlockPos(tx, ty, tz), sandID, 0, 2);
							}
						}
					}
				}
			}

			return true;
		}
	}

	WorldGenShrub::WorldGenShrub(int metaWood, int metaLeaves)
	{
		this->metaWood = metaWood;
		this->metaLeaves = metaLeaves;
	}

	bool WorldGenShrub::generate(World* pWorld, Random& rand, int x, int y, int z)
	{


		for (int blockID; ((blockID = pWorld->getBlockId(BlockPos(x, y, z))) == 0 || blockID == BLOCK_ID_LEAVES) && y > 0; --y)
		{
			;
		}

		int blockId = pWorld->getBlockId(BlockPos(x, y, z));

		if (blockId == BLOCK_ID_DIRT || blockId == BLOCK_ID_GRASS)
		{
			++y;
			setBlockAndMetadata(pWorld, x, y, z, BLOCK_ID_WOOD, metaWood);

			for (int ty = y; ty <= y + 2; ++ty)
			{
				int disY = ty - y;
				int offsetXZ = 2 - disY;

				for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
				{
					int disX = tx - x;

					for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
					{
						int disZ = tz - z;

						if ((Math::Abs(disX) != offsetXZ ||
							Math::Abs(disZ) != offsetXZ ||
							rand.nextInt(2) != 0) &&
							!BlockManager::sOpaqueCubeLookup[pWorld->getBlockId(BlockPos(tx, ty, tz))])
						{
							setBlockAndMetadata(pWorld, tx, ty, tz, BLOCK_ID_LEAVES, metaLeaves);
						}
					}
				}
			}
		}

		return true;
	}

	WorldGenSpikes::WorldGenSpikes(int replaceID)
	{
		replaceID = replaceID;
	}

	bool WorldGenSpikes::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		if (pWorld->isAirBlock(BlockPos(x, y, z)) && pWorld->getBlockId(BlockPos(x, y - 1, z)) == replaceID)
		{
			int offsetY = rand.nextInt(32) + 6;
			int offsetXZ = rand.nextInt(4) + 1;


			for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
			{
				for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
				{
					int disX = tx - x;
					int disZ = tz - z;

					if (disX * disX + disZ * disZ <= offsetXZ * offsetXZ + 1 && pWorld->getBlockId(BlockPos(tx, y - 1, tz)) != replaceID)
					{
						return false;
					}
				}
			}

			for (int ty = y; ty < y + offsetY && ty < 128; ++ty)
			{
				for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
					{
						int disX = tx - x;
						int disZ = tz - z;

						if (disX * disX + disZ * disZ <= offsetXZ * offsetXZ + 1)
						{
							pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_OBSIDIAN, 0, 2);
						}
					}
				}
			}

			/*EntityEnderCrystal var13 = new EntityEnderCrystal(pWorld);
			var13.setLocationAndAngles((double)((float)par3 + 0.5F), (double)(par4 + var6), (double)((float)par5 + 0.5F), rand.nextFloat() * 360.0F, 0.0F);
			pWorld->spawnEntityInWorld(var13);*/
			pWorld->setBlock(BlockPos(x, y + offsetY, z), BLOCK_ID_BEDROCK, 0, 2);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool WorldGenSwamp::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int height;

		for (height = rand.nextInt(4) + 5; pWorld->getBlockMaterial(BlockPos(x, y - 1, z)) == BM_Material::BM_MAT_water; --y)
		{
			;
		}

		bool flag = true;

		if (y >= 1 && y + height + 1 <= 128)
		{

			//check
			for (int ty = y; ty <= y + 1 + height; ++ty)
			{
				i8 offsetXZ = 1;

				if (ty == y)
				{
					offsetXZ = 0;
				}

				if (ty >= y + 1 + height - 2)
				{
					offsetXZ = 3;
				}

				for (int tx = x - offsetXZ; tx <= x + offsetXZ && flag; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ && flag; ++tz)
					{
						if (ty >= 0 && ty < 128)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID != 0 && blockID != BLOCK_ID_LEAVES)
							{
								if (blockID != BLOCK_ID_WATERSTILL && blockID != BLOCK_ID_WATERMOVING)
								{
									flag = false;
								}
								else if (ty > y)
								{
									flag = false;
								}
							}
						}
						else
						{
							flag = false;
						}
					}
				}
			}

			if (!flag)
			{
				return false;
			}
			else
			{
				int blockID = pWorld->getBlockId(BlockPos(x, y - 1, z));

				if ((blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_DIRT) && y < 128 - height - 1)
				{
					setBlock(pWorld, x, y - 1, z, BLOCK_ID_DIRT);


					//generate leave
					for (int ty = y - 3 + height; ty <= y + height; ++ty)
					{
						int var10 = ty - (y + height);
						int offsetXZ = 2 - var10 / 2;

						for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
						{
							int disX = tx - x;

							for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
							{
								int disZ = tz - z;

								if ((Math::Abs(disX) != offsetXZ || Math::Abs(disZ) != offsetXZ || rand.nextInt(2) != 0 && var10 != 0) &&
									!BlockManager::sOpaqueCubeLookup[pWorld->getBlockId(BlockPos(tx, ty, tz))])
								{
									setBlock(pWorld, tx, ty, tz, BLOCK_ID_LEAVES);
								}
							}
						}
					}

					//generate wood
					for (int offsetY = 0; offsetY < height; ++offsetY)
					{
						int blockID = pWorld->getBlockId(BlockPos(x, y + offsetY, z));

						if (blockID == 0 || blockID == BLOCK_ID_LEAVES || blockID == BLOCK_ID_WATERMOVING || blockID == BLOCK_ID_WATERSTILL)
						{
							setBlock(pWorld, x, y + offsetY, z, BLOCK_ID_WOOD);
						}
					}

					for (int ty = y - 3 + height; ty <= y + height; ++ty)
					{
						int var10 = ty - (y + height);
						int offsetXZ = 2 - var10 / 2;

						for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
						{
							for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
							{
								if (pWorld->getBlockId(BlockPos(tx, ty, tz)) == BLOCK_ID_LEAVES)
								{
									if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx - 1, ty, tz)) == 0)
									{
										generateVines(pWorld, tx - 1, ty, tz, 8);
									}

									if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx + 1, ty, tz)) == 0)
									{
										generateVines(pWorld, tx + 1, ty, tz, 2);
									}

									if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx, ty, tz - 1)) == 0)
									{
										generateVines(pWorld, tx, ty, tz - 1, 1);
									}

									if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx, ty, tz + 1)) == 0)
									{
										generateVines(pWorld, tx, ty, tz + 1, 4);
									}
								}
							}
						}
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	void WorldGenSwamp::generateVines(World* pWorld, int x, int y, int z, int metadata)
	{
		setBlockAndMetadata(pWorld, x, y, z, BLOCK_ID_VINE, metadata);
		int length = 4;

		while (true)
		{
			--y;

			if (pWorld->getBlockId(BlockPos(x, y, z)) != 0 || length <= 0)
			{
				return;
			}

			setBlockAndMetadata(pWorld, x, y, z, BLOCK_ID_VINE, metadata);
			--length;
		}
	}

	bool WorldGenTaiga1::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int treeHeight = rand.nextInt(5) + 7;
		int boleHeight = treeHeight - rand.nextInt(2) - 3;
		int leafHeight = treeHeight - boleHeight;
		int leafSize = 1 + rand.nextInt(leafHeight + 1);
		bool flag = true;

		if (y >= 1 && y + treeHeight + 1 <= 128)
		{
			int offsetXZ;

			for (int ty = y; ty <= y + 1 + treeHeight && flag; ++ty)
			{
				bool var12 = true;

				if (ty - y < boleHeight)
				{
					offsetXZ = 0;
				}
				else
				{
					offsetXZ = leafSize;
				}

				for (int tx = x - offsetXZ; tx <= x + offsetXZ && flag; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ && flag; ++tz)
					{
						if (ty >= 0 && ty < 128)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID != 0 && blockID != BLOCK_ID_LEAVES)
							{
								flag = false;
							}
						}
						else
						{
							flag = false;
						}
					}
				}
			}

			if (!flag)
			{
				return false;
			}
			else
			{
				int blockID = pWorld->getBlockId(BlockPos(x, y - 1, z));

				if ((blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_DIRT) && y < 128 - treeHeight - 1)
				{
					setBlock(pWorld, x, y - 1, z, BLOCK_ID_DIRT);
					offsetXZ = 0;

					for (int ty = y + treeHeight; ty >= y + boleHeight; --ty)
					{
						for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
						{
							int disX = tx - x;

							for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
							{
								int disZ = tz - z;

								if ((Math::Abs(disX) != offsetXZ || Math::Abs(disZ) != offsetXZ || offsetXZ <= 0) &&
									!BlockManager::sOpaqueCubeLookup[pWorld->getBlockId(BlockPos(tx, ty, tz))])
								{
									setBlockAndMetadata(pWorld, tx, ty, tz, BLOCK_ID_LEAVES, 1);
								}
							}
						}

						if (offsetXZ >= 1 && ty == y + boleHeight + 1)
						{
							--offsetXZ;
						}
						else if (offsetXZ < leafSize)
						{
							++offsetXZ;
						}
					}

					for (int offsetY = 0; offsetY < treeHeight - 1; ++offsetY)
					{
						int blockID = pWorld->getBlockId(BlockPos(x, y + offsetY, z));

						if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
						{
							setBlockAndMetadata(pWorld, x, y + offsetY, z, BLOCK_ID_WOOD, 1);
						}
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	WorldGenTaiga2::WorldGenTaiga2(bool par1)
		: WorldGenerator(par1)
	{
	}

	bool WorldGenTaiga2::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int treeHeight = rand.nextInt(4) + 6;
		int boleHeight = 1 + rand.nextInt(2);
		int leafHeight = treeHeight - boleHeight;
		int leafSize = 2 + rand.nextInt(2);
		bool flag = true;

		if (y >= 1 && y + treeHeight + 1 <= 256)
		{
			int offsetXZ;

			//check can generate taiga
			for (int ty = y; ty <= y + 1 + treeHeight && flag; ++ty)
			{

				if (ty - y < boleHeight)
				{
					offsetXZ = 0;
				}
				else
				{
					offsetXZ = leafSize;
				}

				for (int tx = x - offsetXZ; tx <= x + offsetXZ && flag; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ && flag; ++tz)
					{
						if (ty >= 0 && ty < 256)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID != 0 && blockID != BLOCK_ID_LEAVES)
							{
								flag = false;
							}
						}
						else
						{
							flag = false;
						}
					}
				}
			}// end of for ty

			if (!flag)
			{
				return false;
			}
			else
			{
				int blockID = pWorld->getBlockId(BlockPos(x, y - 1, z));

				if ((blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_DIRT) && y < 256 - treeHeight - 1)
				{
					setBlock(pWorld, x, y - 1, z, BLOCK_ID_DIRT);
					offsetXZ = rand.nextInt(2);
					int curLeafSize = 1;
					i8 var22 = 0;


					//up --> down 
					//generate leaf
					for (int offsetY = 0; offsetY <= leafHeight; ++offsetY)
					{
						int ty = y + treeHeight - offsetY;

						for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
						{
							int disX = tx - x;

							for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
							{
								int disZ = tz - z;

								if ((Math::Abs(disX) != offsetXZ || Math::Abs(disZ) != offsetXZ || offsetXZ <= 0) &&
									!BlockManager::sOpaqueCubeLookup[pWorld->getBlockId(BlockPos(tx, ty, tz))])
								{
									setBlockAndMetadata(pWorld, tx, ty, tz, BLOCK_ID_LEAVES, 1);
								}
							}
						}

						if (offsetXZ >= curLeafSize)
						{
							offsetXZ = var22;
							var22 = 1;
							++curLeafSize;

							if (curLeafSize > leafSize)
							{
								curLeafSize = leafSize;
							}
						}
						else
						{
							++offsetXZ;
						}
					}

					//generate wood
					int var15 = rand.nextInt(3);//???				
					for (int offsetY = 0; offsetY < treeHeight - var15; ++offsetY)
					{
						int blockID = pWorld->getBlockId(BlockPos(x, y + offsetY, z));

						if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
						{
							setBlockAndMetadata(pWorld, x, y + offsetY, z, BLOCK_ID_WOOD, 1);
						}
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	WorldGenTallGrass::WorldGenTallGrass(int tallGrassID, int tallGrassMetadata)
	{
		this->tallGrassID = tallGrassID;
		this->tallGrassMetadata = tallGrassMetadata;
	}

	bool WorldGenTallGrass::generate(World* pWorld, Random& rand, int x, int y, int z)
	{


		for (int blockID; ((blockID = pWorld->getBlockId(BlockPos(x, y, z))) == 0 || blockID == BLOCK_ID_LEAVES) && y > 0; --y)
		{
			;
		}

		for (int i = 0; i < 128; ++i)
		{
			int tx = x + rand.nextInt(8);
			tx -= rand.nextInt(8);
			int ty = y + rand.nextInt(4);
			ty -= rand.nextInt(4);
			int tz = z + rand.nextInt(8);
			tz -= rand.nextInt(8);

			Block* pBlock = BlockManager::sBlocks[tallGrassID];
			if (pWorld->isAirBlock(BlockPos(tx, ty, tz)) &&
				pBlock->canBlockStay(pWorld, BlockPos(tx, ty, tz)))
			{
				pWorld->setBlock(BlockPos(tx, ty, tz), tallGrassID, tallGrassMetadata, 2);
			}
		}

		return true;
	}

	WorldGenTrees::WorldGenTrees(bool par1)
		: WorldGenerator(par1)
	{
		minTreeHeight = 4;
		metaWood = 0;
		metaLeaves = 0;
		vinesGrow = false;
	}

	WorldGenTrees::WorldGenTrees(bool par1, int minTreeHeight, int metaWood, int metaLeaves, bool vineGrow)
		: WorldGenerator(par1)
	{
		this->minTreeHeight = minTreeHeight;
		this->metaWood = metaWood;
		this->metaLeaves = metaLeaves;
		this->vinesGrow = vineGrow;
	}

	void WorldGenTrees::setParam(int minTreeHeight, int metaWood, int metaLeaves, bool vinesGrow)
	{
		this->minTreeHeight = minTreeHeight;
		this->metaWood = metaWood;
		this->metaLeaves = metaLeaves;
		this->vinesGrow = vinesGrow;
	}

	bool WorldGenTrees::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int treeHeight = rand.nextInt(3) + minTreeHeight;
		bool flag = true;

		if (y >= 1 && y + treeHeight + 1 <= 256)
		{

			for (int ty = y; ty <= y + 1 + treeHeight; ++ty)
			{
				int offsetXZ = 1;

				if (ty == y)
				{
					offsetXZ = 0;
				}

				if (ty >= y + 1 + treeHeight - 2)
				{
					offsetXZ = 2;
				}

				for (int tx = x - offsetXZ; tx <= x + offsetXZ && flag; ++tx)
				{
					for (int tz = z - offsetXZ; tz <= z + offsetXZ && flag; ++tz)
					{
						if (ty >= 0 && ty < 256)
						{
							int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

							if (blockID != 0 && blockID != BLOCK_ID_LEAVES &&
								blockID != BLOCK_ID_GRASS &&
								blockID != BLOCK_ID_DIRT &&
								blockID != BLOCK_ID_WOOD &&
								blockID != BLOCK_ID_WOOD2)
							{
								flag = false;
							}
						}
						else
						{
							flag = false;
						}
					}
				}
			}

			if (!flag)
			{
				return false;
			}
			else
			{
				int blockID = pWorld->getBlockId(BlockPos(x, y - 1, z));

				if ((blockID == BLOCK_ID_GRASS || blockID == BLOCK_ID_DIRT) && y < 256 - treeHeight - 1)
				{
					setBlock(pWorld, x, y - 1, z, BLOCK_ID_DIRT);
					int leavesHeight = 3;
					i8 var19 = 0;

					for (int ty = y - leavesHeight + treeHeight; ty <= y + treeHeight; ++ty)
					{
						int curHeight = ty - (y + treeHeight);
						int offsetXZ = var19 + 1 - curHeight / 2;

						for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
						{
							int disX = tx - x;

							for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
							{
								int disZ = tz - z;

								if (Math::Abs(disX) != offsetXZ || Math::Abs(disZ) != offsetXZ || rand.nextInt(2) != 0 && curHeight != 0)
								{
									int blockID = pWorld->getBlockId(BlockPos(tx, ty, tz));

									if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
									{
										setBlockAndMetadata(pWorld, tx, ty, tz, BLOCK_ID_LEAVES, metaLeaves);
									}
								}
							}
						}
					}

					for (int offsetY = 0; offsetY < treeHeight; ++offsetY)
					{
						int blockID = pWorld->getBlockId(BlockPos(x, y + offsetY, z));

						if (blockID == 0 || blockID == BLOCK_ID_LEAVES)
						{
							setBlockAndMetadata(pWorld, x, y + offsetY, z, BLOCK_ID_WOOD, metaWood);

							if (vinesGrow && offsetY > 0)
							{
								if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x - 1, y + offsetY, z)))
								{
									setBlockAndMetadata(pWorld, x - 1, y + offsetY, z, BLOCK_ID_VINE, 8);
								}

								if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x + 1, y + offsetY, z)))
								{
									setBlockAndMetadata(pWorld, x + 1, y + offsetY, z, BLOCK_ID_VINE, 2);
								}

								if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x, y + offsetY, z - 1)))
								{
									setBlockAndMetadata(pWorld, x, y + offsetY, z - 1, BLOCK_ID_VINE, 1);
								}

								if (rand.nextInt(3) > 0 && pWorld->isAirBlock(BlockPos(x, y + offsetY, z + 1)))
								{
									setBlockAndMetadata(pWorld, x, y + offsetY, z + 1, BLOCK_ID_VINE, 4);
								}
							}
						}
					}

					if (vinesGrow)
					{
						for (int ty = y - 3 + treeHeight; ty <= y + treeHeight; ++ty)
						{
							int curHeight = ty - (y + treeHeight);
							int offsetXZ = 2 - curHeight / 2;

							for (int tx = x - offsetXZ; tx <= x + offsetXZ; ++tx)
							{
								for (int tz = z - offsetXZ; tz <= z + offsetXZ; ++tz)
								{
									if (pWorld->getBlockId(BlockPos(tx, ty, tz)) == BLOCK_ID_LEAVES)
									{
										if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx - 1, ty, tz)) == 0)
										{
											growVines(pWorld, tx - 1, ty, tz, 8);
										}

										if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx + 1, ty, tz)) == 0)
										{
											growVines(pWorld, tx + 1, ty, tz, 2);
										}

										if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx, ty, tz - 1)) == 0)
										{
											growVines(pWorld, tx, ty, tz - 1, 1);
										}

										if (rand.nextInt(4) == 0 && pWorld->getBlockId(BlockPos(tx, ty, tz + 1)) == 0)
										{
											growVines(pWorld, tx, ty, tz + 1, 4);
										}
									}
								}
							}
						}

						if (rand.nextInt(5) == 0 && treeHeight > 5)
						{
							for (int offsetY = 0; offsetY < 2; ++offsetY)
							{
								for (int i = 0; i < 4; ++i)
								{
									if (rand.nextInt(4 - offsetY) == 0)
									{
										int var13 = rand.nextInt(3);
										setBlockAndMetadata(pWorld,
											x + dir_offsetX[dir_rotateOpposite[i]], y + treeHeight - 5 + offsetY, z +
											dir_offsetZ[dir_rotateOpposite[i]], BLOCK_ID_COCOAPLANT, var13 << 2 | i);
									}
								}
							}
						}
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	/** Grows vines downward from the given block for a given length. Args: World, x, starty, z, vine-length */
	void WorldGenTrees::growVines(World* pWorld, int x, int y, int z, int metadata)
	{
		setBlockAndMetadata(pWorld, x, y, z, BLOCK_ID_VINE, metadata);
		int length = 4;

		while (true)
		{
			--y;

			if (pWorld->getBlockId(BlockPos(x, y, z)) != 0 || length <= 0)
			{
				return;
			}

			setBlockAndMetadata(pWorld, x, y, z, BLOCK_ID_VINE, metadata);
			--length;
		}
	}

	bool WorldGenVines::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		int tx = x;
		int tz = z;
		for (; y < 128; ++y)
		{
			if (pWorld->isAirBlock(BlockPos(x, y, z)))
			{
				for (int side = 2; side <= 5; ++side)
				{
					// if (Block.vine.canPlaceBlockOnSide(pWorld, par3, par4, par5, var8))
					BlockVine* pblock = dynamic_cast<BlockVine*>(BlockManager::vine);
					if (pblock->canPlaceBlockOnSide(pWorld, BlockPos(x, y, z), side))
					{
						pWorld->setBlock(BlockPos(x, y, z), BLOCK_ID_VINE, 1 << dir_facingToDirection[oppositeSide[side]], 2);
						break;
					}
				}
			}
			else
			{
				x = tx + rand.nextInt(4);
				x -= rand.nextInt(4);
				z = tz + rand.nextInt(4);
				z -= rand.nextInt(4);
			}
		}

		return true;
	}

	bool WorldGenWaterlily::generate(World* pWorld, Random& rand, int x, int y, int z)
	{
		for (int i = 0; i < 10; ++i)
		{
			int tx = x + rand.nextInt(8);
			tx -= rand.nextInt(8);
			int ty = y + rand.nextInt(4);
			ty -= rand.nextInt(4);
			int tz = z + rand.nextInt(8);
			tz -= rand.nextInt(8);

			BlockLilyPad* lilypad = (BlockLilyPad*)BlockManager::waterlily;
			if (pWorld->isAirBlock(BlockPos(tx, ty, tz)) && lilypad->canPlaceBlockAt(pWorld, BlockPos(tx, ty, tz)))
			{
				pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_WATERLILY, 0, 2);
			}
		}

		return true;
	}

	WorldGeneratorBonusChest::WorldGeneratorBonusChest(WeightedRandomChestContent** par1ArrayOfWeightedRandomChestContent, int par2)
	{
		theBonusChestGenerator = par1ArrayOfWeightedRandomChestContent;
		itemsToGenerateInBonusChest = par2;
	}

	bool WorldGeneratorBonusChest::generate(World* pWorld, Random& rand, int x, int y, int z)
	{

		for (int blockID; ((blockID = pWorld->getBlockId(BlockPos(x, y, z))) == 0 || blockID == BLOCK_ID_LEAVES) && y > 1; --y)
		{
			;
		}

		if (y < 1)
		{
			return false;
		}
		else
		{
			++y;

			for (int i = 0; i < 4; ++i)
			{
				int tx = x + rand.nextInt(4);
				tx -= rand.nextInt(4);
				int ty = y + rand.nextInt(3);
				ty -= rand.nextInt(3);
				int tz = z + rand.nextInt(4);
				tz -= rand.nextInt(4);

				if (pWorld->isAirBlock(BlockPos(tx, ty, tz)) && pWorld->doesBlockHaveSolidTopSurface(BlockPos(tx, ty - 1, tz)))
				{
					pWorld->setBlock(BlockPos(tx, ty, tz), BLOCK_ID_CHEST, 0, 2);
					/*TileEntityChest var11 = (TileEntityChest)pWorld->getBlockTileEntity(var8, var9, var10);

					if (var11 != null && var11 != null)
					{
					WeightedRandomChestContent.generateChestContents(rand, this.theBonusChestGenerator, var11, this.itemsToGenerateInBonusChest);
					}*/

					if (pWorld->isAirBlock(BlockPos(tx - 1, ty, tz)) && pWorld->doesBlockHaveSolidTopSurface(BlockPos(tx - 1, ty - 1, tz)))
					{
						pWorld->setBlock(BlockPos(tx - 1, ty, tz), BLOCK_ID_TORCH_WOOD, 0, 2);
					}

					if (pWorld->isAirBlock(BlockPos(tx + 1, ty, tz)) && pWorld->doesBlockHaveSolidTopSurface(BlockPos(tx - 1, ty - 1, tz)))
					{
						pWorld->setBlock(BlockPos(tx + 1, ty, tz), BLOCK_ID_TORCH_WOOD, 0, 2);
					}

					if (pWorld->isAirBlock(BlockPos(tx, ty, tz - 1)) && pWorld->doesBlockHaveSolidTopSurface(BlockPos(tx - 1, ty - 1, tz)))
					{
						pWorld->setBlock(BlockPos(tx, ty, tz - 1), BLOCK_ID_TORCH_WOOD, 0, 2);
					}

					if (pWorld->isAirBlock(BlockPos(tx, ty, tz + 1)) && pWorld->doesBlockHaveSolidTopSurface(BlockPos(tx - 1, ty - 1, tz)))
					{
						pWorld->setBlock(BlockPos(tx, ty, tz + 1), BLOCK_ID_TORCH_WOOD, 0, 2);
					}

					return true;
				}
			}

			return false;
		}
	}

}