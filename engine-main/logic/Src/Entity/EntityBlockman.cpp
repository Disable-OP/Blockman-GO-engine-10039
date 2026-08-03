#include "EntityBlockman.h"
#include "Block/BlockManager.h"
#include "Block/Block.h"
#include "Block/Blocks.h"
#include "World/WorldProvider.h"
#include "DamageSource.h"
#include "Item/Item.h"
#include "Setting/ActorSizeSetting.h"

using namespace  LORD;
namespace BLOCKMAN
{
	//EntityBlockman::matrix[10][2][3] = { { { 0, 0, -1 },{ 0, 0, 1 } },{ { -1, 0, 0 },{ 1, 0, 0 } },{ { -1, -1, 0 },{ 1, 0, 0 } },{ { -1, 0, 0 },{ 1, -1, 0 } },{ { 0, 0, -1 },{ 0, -1, 1 } },{ { 0, -1, -1 },{ 0, 0, 1 } },{ { 0, 0, 1 },{ 1, 0, 0 } },{ { 0, 0, 1 },{ -1, 0, 0 } },{ { 0, 0, -1 },{ -1, 0, 0 } },{ { 0, 0, -1 },{ 1, 0, 0 } } };

	EntityBlockman::~EntityBlockman()
	{
	}

	void EntityBlockman::moveOnOtherBlock()
	{
		if (motion.x < -currentMaxSpeed)
		{
			motion.x= -currentMaxSpeed;
		}

		if (motion.x > currentMaxSpeed)
		{
			motion.x = currentMaxSpeed;
		}

		if (motion.z < -currentMaxSpeed)
		{
			motion.z = -currentMaxSpeed;
		}

		if (motion.z > currentMaxSpeed)
		{
			motion.z = currentMaxSpeed;
		}

		if (onGround)
		{
			motion *= 0.5;
		}

		if (canFall())
		{
			moveEntity(motion);
		}

		if (!onGround)
		{
			motion *= 0.949999988079071;
		}
	}

	void EntityBlockman::updateOnTrack(int posX, int posY, int posZ, int blockId, int meta)
	{
		fallDistance = 0.0F;
		Vector3 var10 = getPos(position.x, position.y, position.z);
		position.y = (float)posY;
		bool var11 = false;
		bool var12 = false;

		if (blockId == BlockManager::railPowered->getBlockID())
		{
			var11 = (meta & 8) != 0;
			var12 = !var11;
		}

		if (((BlockRailBase*)BlockManager::sBlocks[blockId])->isPowered())
		{
			meta &= 7;
		}

		if (meta >= 2 && meta <= 5)
		{
			position.y = (float)posY + 1;
		}

		auto var13 = matrix[meta];
		float var14 = (float)(var13[1][0] - var13[0][0]);
		float var16 = (float)(var13[1][2] - var13[0][2]);
		float var18 = std::sqrt(var14 * var14 + var16 * var16);
		float var20 = motion.x * var14 + motion.z * var16;

		if (var20 < 0.0)
		{
			var14 = -var14;
			var16 = -var16;
		}

		float var22 = std::sqrt(motion.x * motion.x + motion.z * motion.z);

		if (var22 > 2.0f)
		{
			var22 = 2.0f;
		}

		motion.x = var22 * var14 / var18;
		motion.z = var22 * var16 / var18;
		float var24;
		float var26;
		float var28;
		float var30;

		/*if (riddenByEntity && dynamic_cast<EntityLivingBase*>(riddenByEntity))
		{
			var24 = dynamic_cast<EntityLivingBase*>(riddenByEntity)->moveForward;

			if (var24 > 0)
			{
				var26 = -std::sin((float)(riddenByEntity->rotationYaw * (float)Math::PI / 180.0F));
				var28 = std::cos((float)(riddenByEntity->rotationYaw * (float)Math::PI / 180.0F));
				var30 = motion.x * motion.x + motion.z * motion.z;

				if (var30 < 0.01f)
				{
					motion.x += var26 * 0.1f;
					motion.z += var28 * 0.1f;
					var12 = false;
				}
			}
		}*/

		if (var12)
		{
			var24 = std::sqrt(motion.x * motion.x + motion.z * motion.z);

			if (var24 < 0.03f)
			{
				motion *= 0;
			}
			else
			{
				motion.x *= 0.5f;
				motion.y *= 0.0f;
				motion.z *= 0.5f;
			}
		}

		var24 = 0.0f;
		var26 = (float)posX + 0.5f + (float)var13[0][0] * 0.5f;
		var28 = (float)posZ + 0.5f + (float)var13[0][2] * 0.5f;
		var30 = (float)posX + 0.5f + (float)var13[1][0] * 0.5f;
		float var32 = (float)posZ + 0.5f + (float)var13[1][2] * 0.5f;
		var14 = var30 - var26;
		var16 = var32 - var28;
		float var34;
		float var36;

		if (var14 == 0.0f)
		{
			position.x = (float)posX + 0.5f;
			var24 = position.z - (float)posZ;
		}
		else if (var16 == 0.0f)
		{
			position.z = (float)posZ + 0.5f;
			var24 = position.x - (float)posX;
		}
		else
		{
			var34 = position.x - var26;
			var36 = position.z - var28;
			var24 = (var34 * var14 + var36 * var16) * 2.0f;
		}

		position.x = var26 + var14 * var24;
		position.z = var28 + var16 * var24;
		setPosition(Vector3(position.x, position.y + yOffset, position.z));
		var34 = motion.x;
		var36 = motion.z;

		if (riddenByEntity)
		{
			var34 *= 0.75f;
			var36 *= 0.75f;
		}

		if (var34 < -currentMaxSpeed)
		{
			var34 = -currentMaxSpeed;
		}

		if (var34 > currentMaxSpeed)
		{
			var34 = currentMaxSpeed;
		}

		if (var36 < -currentMaxSpeed)
		{
			var36 = -currentMaxSpeed;
		}

		if (var36 > currentMaxSpeed)
		{
			var36 = currentMaxSpeed;
		}

		if (canFall())
		{
			moveEntity(Vector3(var34, 0.0f, var36));
		}

		if (var13[0][1] != 0 && (int)Math::Floor(position.x) - posX == var13[0][0] && (int)Math::Floor(position.z) - posZ == var13[0][2])
		{
			setPosition(Vector3(position.x, position.y + var13[0][1], position.z));
		}
		else if (var13[1][1] != 0 && (int)Math::Floor(position.x) - posX == var13[1][0] && (int)Math::Floor(position.z) - posZ == var13[1][2])
		{
			setPosition(Vector3(position.x, position.y + var13[1][1], position.z));
		}

		applyDrag();

		Vector3 var38 = getPos(position.x, position.y, position.z);

		if (var38 != Vector3::ZERO && var10 != Vector3::ZERO)
		{
			float var39 = (var10.y - var38.y) * 0.05f;
			var22 = std::sqrt(motion.x * motion.x + motion.z * motion.z);

			if (var22 > 0.0f)
			{
				motion.x = motion.x / var22 * (var22 + var39);
				motion.z = motion.z / var22 * (var22 + var39);
			}
			setPosition(Vector3(position.x, var38.y, position.z));
		}

		int var45 = (int)Math::Floor(position.x);
		int var40 = (int)Math::Floor(position.z);

		if (var45 != posX || var40 != posZ)
		{
			var22 = std::sqrt(motion.x * motion.x + motion.z * motion.z);
			motion.x = var22 * (var45 - posX);
			motion.z = var22 * (var40 - posZ);
		}

		if (var11)
		{
			float var41 = std::sqrt(motion.x * motion.x + motion.z * motion.z);

			if (var41 > 0.01f)
			{
				float var43 = 0.06f;
				motion.x += motion.x / var41 * var43;
				motion.z += motion.z / var41 * var43;
			}
			else if (meta == 1)
			{
				if (world->isBlockNormalCube(BlockPos(posX - 1, posY, posZ)))
				{
					motion.x = 0.02f;
				}
				else if (world->isBlockNormalCube(BlockPos(posX + 1, posY, posZ)))
				{
					motion.x = -0.02f;
				}
			}
			else if (meta == 0)
			{
				if (world->isBlockNormalCube(BlockPos(posX, posY, posZ - 1)))
				{
					motion.x = 0.02f;
				}
				else if (world->isBlockNormalCube(BlockPos(posX, posY, posZ + 1)))
				{
					motion.z = -0.02f;
				}
			}
		}
	}

	void EntityBlockman::applyDrag()
	{
		if (riddenByEntity)
		{
			motion.x *= 0.996999979019165f;
			motion.y *= 0.0f;
			motion.z *= 0.996999979019165f;
		}
		else
		{
			motion.x *= 0.9599999785423279f;
			motion.y *= 0.0f;
			motion.z *= 0.9599999785423279f;
		}
	}

	void EntityBlockman::setDead()
	{
		Entity::setDead();
	}

	void EntityBlockman::onUpdate()
	{
		prevPos = position;

		Entity::onUpdate();

		if (getRollingAmplitude() > 0)
		{
			setRollingAmplitude(getRollingAmplitude() - 1);
		}

		if (getDamage() > 0.0F)
		{
			setDamage(getDamage() - 1.0F);
		}

		int y = 0;

		if (!world->m_isClient)
		{
			y = 1;

			if (inPortal)
			{
				/*if (ridingEntity && portalCounter++ >= y)
				{
					portalCounter = y;
					timeUntilPortal = getPortalCooldown();
					int var3;
					if (world->m_provider->dimensionId == -1)
					{
						var3 = 0;
					}
					else
					{
						var3 = -1;
					}

					travelToDimension(var3);
				}*/

				inPortal = false;
			}
			else
			{
				if (portalCounter > 0)
				{
					portalCounter -= 4;
				}

				if (portalCounter < 0)
				{
					portalCounter = 0;
				}
			}

			if (timeUntilPortal > 0)
			{
				--timeUntilPortal;
			}
		}

		/*if (world->m_isClient)
		{
			if (turnProgress > 0)
			{
				Vector3 pos = position + (blockmanPos - position) / (float) turnProgress;
				float deltaYaw = blockmanYaw - rotationYaw ;
				deltaYaw = Math::Mod(deltaYaw,360.f);
				deltaYaw = deltaYaw > 180.f ? deltaYaw - 360.f : deltaYaw;
				deltaYaw = deltaYaw < -180.f ? deltaYaw + 360.f : deltaYaw;
				rotationYaw =rotationYaw + deltaYaw / turnProgress;
				rotationPitch = rotationPitch + (blockmanPitch - rotationPitch) / turnProgress;
				--turnProgress;
				setPosition(pos);
				setRotation(rotationYaw, rotationPitch);
			}
			else
			{
				setPosition(position);
				setRotation(rotationYaw, rotationPitch);
			}
		}*/
		//else
		//{

		motion.y -= 0.03999999910593033f;
		int x = (int)Math::Floor(position.x);
		y = (int)Math::Floor(position.y);
		int z = (int)Math::Floor(position.z);

		if (BlockRailBase::isRailBlockAt(world, Vector3i(x, y - 1, z)))
		{
			--y;
		}

		int blockId = world->getBlockId(Vector3i(x, y, z));
		if (checkBlcok(blockId))
			return;

		if (BlockRailBase::isRailBlock(blockId))
		{
			int meta = world->getBlockMeta(Vector3i(x, y, z));
			updateOnTrack(x, y, z, blockId, meta);

			if (blockId == BlockManager::railActivator->getBlockID())
			{
				onActivatorRailPass(Vector3i(x, y, z), (meta & 8) != 0);
			}
		}
		else
		{
			moveOnOtherBlock();
		}

		doBlockCollisions();
		rotationPitch = 0.0F;
		float var22 = prevPos.x - position.x;
		float var11 = prevPos.z - position.z;
		
		if (var22 * var22 + var11 * var11 > 0.00005)
		{
			rotationYaw = (float)(std::atan2(var11, var22) * 180.0f / Math::PI);
			rotationYaw += 90;
		}

		/*float var13 = blockmanYaw - rotationYaw;
		var13 = Math::Mod(var13, 360.f);
		var13 = var13 > 180.f ? var13 - 360.f : var13;
		var13 = var13 < -180.f ? var13 + 360.f : var13;

		if (var13 < -170.0f || var13 >= 170.0f)
		{
			rotationYaw += 180.0F;
			isInReverse = !isInReverse;
		}

		setRotation(rotationYaw, rotationPitch);
		*/
			
		if (!world->m_isClient)
		{
			EntityArr var15 = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.expand((0.20000000298023224f, 0.0f, 0.20000000298023224f)));
			for (auto entity : var15)
			{
				if (entity->canBeCollidedWith() && dynamic_cast<EntityBlockman*>(entity))
				{
					dynamic_cast<EntityBlockman*>(entity)->applyEntityCollision(this);
				}
			}

			/*if (riddenByEntity && riddenByEntity->isDead)
			{
				if (riddenByEntity->ridingEntity == this)
				{
					riddenByEntity->ridingEntity = nullptr;
				}

				riddenByEntity = nullptr;
			}*/
		}
		//}
	}

	Vector3i EntityBlockman::getPosOffset(float posX, float posY, float posZ, float offset)
	{
		int x = (int)Math::Floor(posX);
		int y = (int)Math::Floor(posY);
		int z = (int)Math::Floor(posZ);

		if (BlockRailBase::isRailBlockAt(world, Vector3i(x, y - 1, z)))
		{
			--y;
		}

		int blockId = world->getBlockId(Vector3i(x, y, z));

		if (!BlockRailBase::isRailBlock(blockId))
		{
			return Vector3i::ZERO;
		}
		else
		{
			int blockMeta = world->getBlockMeta(Vector3i(x, y, z));

			if (((BlockRailBase*)BlockManager::sBlocks[blockId])->isPowered())
			{
				blockMeta &= 7;
			}

			posY = (float)y;

			if (blockMeta >= 2 && blockMeta <= 5)
			{
				posY = (float)(y + 1);
			}

			auto var14 = matrix[blockMeta];
			float var15 = (float)(var14[1][0] - var14[0][0]);
			float var17 = (float)(var14[1][2] - var14[0][2]);
			float var19 = std::sqrt(var15 * var15 + var17 * var17);
			var15 /= var19;
			var17 /= var19;
			posX += var15 * offset;
			posZ += var17 * offset;

			if (var14[0][1] != 0 && (int)Math::Floor(posX) - x == var14[0][0] && (int)Math::Floor(posZ) - z == var14[0][2])
			{
				posY += (float)var14[0][1];
			}
			else if (var14[1][1] != 0 && (int)Math::Floor(posX) - x == var14[1][0] && (int)Math::Floor(posZ) - z == var14[1][2])
			{
				posY += (float)var14[1][1];
			}

			return getPos(posX, posY, posZ);
		}
	}

	Vector3i EntityBlockman::getPos(float posX, float posY, float posZ)
	{
		int x = (int)Math::Floor(posX);
		int y = (int)Math::Floor(posY);
		int z = (int)Math::Floor(posZ);

		if (BlockRailBase::isRailBlockAt(world, Vector3i(x, y - 1, z)))
		{
			--y;
		}

		int blockId = world->getBlockId(Vector3i(x, y, z));

		if (BlockRailBase::isRailBlock(blockId))
		{
			int blockMeta = world->getBlockMeta(Vector3i(x, y, z));
			posY = (float)y;

			if (((BlockRailBase*)BlockManager::sBlocks[blockId])->isPowered())
			{
				blockMeta &= 7;
			}

			if (blockMeta >= 2 && blockMeta <= 5)
			{
				posY = (float)(y + 1);
			}

			auto var12 = matrix[blockMeta];
			float var13 = 0.0f;
			float var15 = (float)x + 0.5f + (float)var12[0][0] * 0.5f;
			float var17 = (float)y + 0.5f + (float)var12[0][1] * 0.5f;
			float var19 = (float)z + 0.5f + (float)var12[0][2] * 0.5f;
			float var21 = (float)x + 0.5f + (float)var12[1][0] * 0.5f;
			float var23 = (float)y + 0.5f + (float)var12[1][1] * 0.5f;
			float var25 = (float)z + 0.5f + (float)var12[1][2] * 0.5f;
			float var27 = var21 - var15;
			float var29 = (var23 - var17) * 2.0f;
			float var31 = var25 - var19;

			if (var27 == 0.0f)
			{
				posX = (float)x + 0.5f;
				var13 = posZ - (float)z;
			}
			else if (var31 == 0.0f)
			{
				posZ = (float)z + 0.5f;
				var13 = posX - (float)x;
			}
			else
			{
				float var33 = posX - var15;
				float var35 = posZ - var19;
				var13 = (var33 * var27 + var35 * var31) * 2.0f;
			}

			posX = var15 + var27 * var13;
			posY = var17 + var29 * var13;
			posZ = var19 + var31 * var13;

			if (var29 < 0.0f)
			{
				++posY;
			}

			if (var29 > 0.0f)
			{
				posY += 0.5f;
			}
			return  Vector3i((int)posX, (int)posY, (int)posZ);
		}
		else
		{
			return Vector3i::ZERO;
		}
	}

	Box EntityBlockman::getCollisionBox(Entity * par1Entity)
	{
		return par1Entity->canBeCollidedWith() ? par1Entity->boundingBox : boundingBox;
	}

	bool EntityBlockman::attackEntityFrom(DamageSource * source, float amount)
	{
		return false;
	}
	void EntityBlockman::killBlockman(DamageSource * source)
	{
		setDead();
		ItemStack* itemStack = LordNew ItemStack(Item::blockmanEmpty, 1);
		ItemStackPtr var2 = std::shared_ptr<ItemStack>();
		var2.reset(itemStack);
		if (entityName.length() > 0)
		{
			var2->setItemName(entityName);
		}
		entityDropItem(var2, 0.0F);
	}
	void EntityBlockman::performHurtAnimation()
	{
		setRollingDirection(-getRollingDirection());
		setRollingAmplitude(10);
		setDamage(getDamage() + getDamage() * 10.0F);
	}

	void EntityBlockman::applyEntityCollision(Entity * par1Entity)
	{
		if (!world->m_isClient)
		{
			if (par1Entity != riddenByEntity)
			{
				if (dynamic_cast<EntityLivingBase*>(par1Entity) && !(dynamic_cast<EntityLivingBase*>(par1Entity)) && getBlockmanType() == EntityBlockmanType::BLOCKMAN_NORMAL && motion.x * motion.x + motion.z * motion.z > 0.01f && riddenByEntity && par1Entity->ridingEntity)
				{
					par1Entity->mountEntity(this);
				}

				float var2 = par1Entity->position.x - position.x;
				float var4 = par1Entity->position.z - position.z;
				float var6 = var2 * var2 + var4 * var4;

				if (var6 >= 9.999999747378752E-5f)
				{
					var6 = (float)std::sqrt(var6);
					var2 /= var6;
					var4 /= var6;
					float var8 = 1.0f / var6;

					if (var8 > 1.0f)
					{
						var8 = 1.0f;
					}

					var2 *= var8;
					var4 *= var8;
					var2 *= 0.10000000149011612f;
					var4 *= 0.10000000149011612f;
					var2 *= (float)(1.0F - entityCollisionReduction);
					var4 *= (float)(1.0F - entityCollisionReduction);
					var2 *= 0.5f;
					var4 *= 0.5f;

					if (dynamic_cast<EntityBlockman*>(par1Entity))
					{
						float var10 = par1Entity->position.x - position.x;
						float var12 = par1Entity->position.z - position.z;
						Vector3 var14 = Vector3(var10, 0.0f, var12);
						Vector3 var15 = Vector3((float)std::cos(rotationYaw * (float)Math::PI / 180.0F), 0.0f, (float)std::sin(rotationYaw * (float)Math::PI / 180.0F));
						float var16 = var14.absdot(var15);

						if (var16 < 0.800000011920929f)
						{
							return;
						}

						float var18 = par1Entity->motion.x + motion.x;
						float var20 = par1Entity->motion.z + motion.z;

						if ((int)((EntityBlockman*)par1Entity)->getBlockmanType() == 2 && (int) getBlockmanType() != 2)
						{
							motion.x *= 0.20000000298023224f;
							motion.z *= 0.20000000298023224f;
							addVelocity(Vector3(par1Entity->motion.x - var2, 0.0f, par1Entity->motion.z - var4));
							par1Entity->motion.x *= 0.949999988079071f;
							par1Entity->motion.z *= 0.949999988079071f;
						}
						else if ((int)((EntityBlockman*)par1Entity)->getBlockmanType() != 2 && (int) getBlockmanType() == 2)
						{
							par1Entity->motion.x *= 0.20000000298023224f;
							par1Entity->motion.z *= 0.20000000298023224f;
							par1Entity->addVelocity(Vector3(motion.x + var2, 0.0f, motion.z + var4));
							motion.x *= 0.949999988079071f;
							motion.z *= 0.949999988079071f;
						}
						else
						{
							var18 /= 2.0f;
							var20 /= 2.0f;
							motion.x *= 0.20000000298023224f;
							motion.z *= 0.20000000298023224f;
							addVelocity(Vector3(var18 - var2, 0.0f, var20 - var4));
							par1Entity->motion.x *= 0.20000000298023224f;
							par1Entity->motion.z *= 0.20000000298023224f;
							par1Entity->addVelocity(Vector3(var18 + var2, 0.0f, var20 + var4));
						}
					}
					else
					{
						addVelocity(Vector3 (-var2, 0.0f, -var4));
						par1Entity->addVelocity(Vector3(var2 / 4.0f, 0.0f, var4 / 4.0f));
					}
				}
			}
		}
	}

	void EntityBlockman::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
	{
		Entity::setPositionAndRotation2(pos, yaw, pitch, increment);
		/*blockmanPos = pos;
		blockmanYaw = yaw;
		blockmanPitch = pitch;
		turnProgress = increment + 2;
		motion = velocity;*/
	}
	void EntityBlockman::setVelocity(Vector3 v)
	{
		velocity = v;
	}
	void EntityBlockman::setDamage(float damage)
	{
		this->damage = damage;
	}
	float EntityBlockman::getDamage()
	{
		return damage;
	}

	Block * EntityBlockman::getDisplayTile()
	{
		if (!hasDisplayTile())
		{
			return getDefaultDisplayTile();
		}
		else
		{
			int var1 = displayTile & 65535;
			return var1 > 0 && BlockManager::sBlocks[var1] ? BlockManager::sBlocks[var1] : nullptr;
		}
	}

	int EntityBlockman::getDisplayTileData()
	{
		return !hasDisplayTile() ? getDefaultDisplayTileData() : displayTileData >> 16;
	}

	int EntityBlockman::getDefaultDisplayTileData()
	{
		return 6;
	}

	int EntityBlockman::getDisplayTileOffset()
	{
		return !hasDisplayTile() ? getDefaultDisplayTileOffset() : displayTileOffset;
	}

	void EntityBlockman::setDisplayTile(int par1)
	{
		displayTile = par1 & 65535 | getDisplayTileData() << 16;
	}

	void EntityBlockman::setDisplayTileData(int par1)
	{
		Block* var2 = getDisplayTile();
		int var3 = var2 ? 0 : var2->getBlockID();
		displayTileData = var3 & 65535 | par1 << 16;
		setHasDisplayTile(true);

	}

	void EntityBlockman::setDisplayTileOffset(int par1)
	{
		displayTileOffset = par1;
		setHasDisplayTile(true);
	}

	String EntityBlockman::getEntityName()
	{
		return entityName.length() > 0 ? entityName : Entity::getEntityName();
	}

	void EntityBlockman::travelToDimension(int dimension)
	{
	}

	void EntityBlockman::setActorName(String actorName)
	{
		m_actorName = actorName;
		ActorSize* pSize = ActorSizeSetting::getActorSize(m_actorName);
		setSize(pSize->width, pSize->length, pSize->height);
		setPosition(position);
	}

}
