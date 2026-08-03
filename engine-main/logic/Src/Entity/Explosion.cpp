#include "Explosion.h"
#include "Entity.h"
#include "EntityLivingBase.h"
#include "EntityPlayer.h"
#include "EntityMerchant.h"
#include "EntityRankNpc.h"
#include "DamageSource.h"
#include "Enchantment.h"

#include "Util/Random.h"
#include "World/World.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "EntityTNTPrimed.h"
#include "EntityTNTThrowable.h"
#include "EntityGrenade.h"
#include "EntitySessionNpc.h"
#include "EntityActorNpc.h"

#include "Script/Event/LogicScriptEvents.h"
#include "Entity/EntityItem.h"

namespace BLOCKMAN
{

	Explosion::Explosion(World* pWorld, Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float size)
		: worldObj(pWorld)
		, exploder(exploder)
		, explosivePlacedBy(explosivePlacedBy)
		, explosionSize(size)
		, explosionPos(pos)
		, isFlaming(false)
		, isSmoking(true)
	{
		rand = LordNew Random();
	}

	Explosion::Explosion(World* pWorld, Entity* exploder, EntityLivingBase* explosivePlacedBy, const Vector3& pos, float size, BlockPosArr blocksPos)
		: Explosion(pWorld, exploder, explosivePlacedBy, pos, size)
	{
		affectedBlocksPos = blocksPos;
	}

	Explosion::~Explosion()
	{
		LordSafeDelete(rand);
	}

	void Explosion::doExplosionA(bool isBreakBlock, bool isHurt, bool isHurtEntityItem, float custom_explosionSize)
	{
		float var1 = explosionSize;
		explosionSize = custom_explosionSize * explosionSize;
		
		Vector3iSet collision;
		if (isBreakBlock)
		{
			for (int i = 0; i < 16; ++i)
			{
				for (int j = 0; j < 16; ++j)
				{
					for (int k = 0; k < 16; ++k)
					{
						if (i == 0 || i == 15 || j == 0 || j == 15 || k == 0 || k == 15)
						{
							Vector3 dir;
							dir.x = (float)i / (float)15.0F * 2.0F - 1.0F;
							dir.y = (float)j / (float)15.0F * 2.0F - 1.0F;
							dir.z = (float)k / (float)15.0F * 2.0F - 1.0F;
							dir.normalize();
							float range = explosionSize * (0.7F + worldObj->m_Rand.nextFloat() * 0.6F);
							Vector3 epos = explosionPos;
							for (float fi = 0.3F; range > 0.0F; range -= fi * 0.75F)
							{
								BlockPos pos = epos.getFloor();
								int blockID = worldObj->getBlockId(pos);

								if (SCRIPT_EVENT::BlockTNTExplodeBreakBlockEvent::invoke(pos, blockID))
								{
									if (blockID > 0)
									{
										Block* pBlock = BlockManager::sBlocks[blockID];
										float f = 0.f;
										if (exploder)
											f = exploder->getExplosionResistance(this, worldObj, pos, pBlock);
										else
											f = pBlock->getExplosionResistance(NULL);
										range -= (f + 0.3F) * fi;
									}

									if (range > 0.0F && (exploder == NULL || exploder->verifyExplosion(this, worldObj, pos, blockID, range)))
									{
										collision.insert(pos);
									}
								}

								epos += dir * fi;
							}
						}
					}
				}
			}
		}
		affectedBlocksPos.clear();
		affectedBlocksPos.insert(affectedBlocksPos.end(), collision.begin(), collision.end());

		explosionSize *= 2.0F;
		Vector3 exploseBound(explosionSize + 1.f, explosionSize + 1.f, explosionSize + 1.f);
		Vector3i iBegin = (explosionPos - exploseBound).getFloor();
		Vector3i iEnd = (explosionPos + exploseBound).getFloor();
		EntityArr entities = worldObj->getEntitiesWithinAABBExcludingEntity(exploder, Box(iBegin, iEnd));
		Vector3 pos = explosionPos;

		if (isHurt)
		{
			for (EntityArr::iterator it = entities.begin(); it != entities.end(); ++it)
			{
				Entity* pEntity = *it;

				auto entityIsItem = dynamic_cast<EntityItem*>(pEntity);
				if (entityIsItem && !isHurtEntityItem)
				{
					continue;
				}

				if (!pEntity->isImmuneToExplosions())
				{
					float affectRatio = pEntity->getDistance(explosionPos) / explosionSize;

					if (affectRatio <= 1.0f)
					{
						Vector3 disVec = pEntity->position - explosionPos;
						disVec.y += pEntity->getEyeHeight();

						float len = disVec.len();

						if (len != 0.0f)
						{
							disVec /= len;
							float density = worldObj->getBlockDensity(pos, pEntity->boundingBox);
							float amount = (1.0f - affectRatio) * density;
							pEntity->attackEntityFrom(DamageSource::setExplosionSource(this), (float)((int)((amount * amount + amount) / 2.0f * 8.0f * explosionSize + 1.0f)));
							float reduction = EnchantmentProtection::getBlastDamageReduction(pEntity, amount);
							pEntity->motion += disVec * reduction;
							EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(pEntity);
							if (pplayer)
							{
								playerKnockbackMap.insert(std::make_pair(pEntity, disVec * amount));
							}
						}
					}
				}
			}
		}
		explosionSize = var1;
	}

	void Explosion::doExplosionB(bool spawnParticles, float custom_explosionSize)
	{
		float old_explosionSize = explosionSize;
		explosionSize = custom_explosionSize * explosionSize;
		
		//worldObj->playSoundEffect(explosionPos, "random.explode", 4.0F, (1.0F + (worldObj->m_Rand.nextFloat() - worldObj->m_Rand.nextFloat()) * 0.2F) * 0.7F);
		worldObj->playSoundEffectByType(explosionPos, ST_Explosion);

		if (explosionSize >= 2.0F && isSmoking)
		{
			worldObj->spawnParticle("hugeexplosion", explosionPos, Vector3::UNIT_X);
		}
		else
		{
			worldObj->spawnParticle("largeexplode", explosionPos, Vector3::UNIT_X);
		}

		int blockID;

		if (isSmoking)
		{
			vector<Vector3i>::type::iterator it = affectedBlocksPos.begin();
			for (; it != affectedBlocksPos.end(); ++it)
			{
				Vector3i pos = *it;
				blockID = worldObj->getBlockId(pos);

				/*
				if (spawnParticles)
				{
				Vector3 vpos = Vector3(pos) + worldObj->m_Rand.nextVector();
				Vector3 vdel = vpos - explosionPos;
				float dLen = vdel.len();
				vdel /= dLen;
				float size = 0.5f / (dLen / explosionSize + 0.1f);
				size *= worldObj->m_Rand.nextFloat() * worldObj->m_Rand.nextFloat() + 0.3F;
				vdel *= size;
				worldObj->spawnParticle("explode", (vpos + explosionPos * 1.f) / 2.f, vdel);
				worldObj->spawnParticle("smoke", vpos, vdel);
				}
				*/

				if (blockID > 0)
				{
					Block* pBlock = BlockManager::sBlocks[blockID];
					LordAssert(pBlock);
					if (pBlock->canDropFromExplosion(this))
					{
						pBlock->dropBlockAsItemWithChance(worldObj, pos, worldObj->getBlockMeta(pos), 1.0F / explosionSize, 0);
					}

					worldObj->setBlock(pos, 0, 0, 3);
					pBlock->onBlockDestroyedByExplosion(worldObj, pos, this);
				}
			}
		}

		if (isFlaming)
		{
			vector<Vector3i>::type::iterator it = affectedBlocksPos.begin();
			for (; it != affectedBlocksPos.end(); ++it)
			{
				Vector3i pos = *it;
				blockID = worldObj->getBlockId(pos);
				int blockIDUnder = worldObj->getBlockId(pos.getNegY());

				if (blockID == 0 && BlockManager::sOpaqueCubeLookup[blockIDUnder] && rand->nextInt(3) == 0)
				{
					worldObj->setBlock(pos, BLOCK_ID_FIRE);
				}
			}
		}

		explosionSize = old_explosionSize;
	}

}