#include "EntityLivingBase.h"
#include "CombatTracker.h"
#include "DamageSource.h"
#include "EntityPlayer.h"
#include "Enchantment.h"
#include "EntityXPOrb.h"
#include "Common.h"

#include "Object/Root.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Item/ItemStack.h"
#include "Item/Items.h"
#include "Item/Potion.h"
#include "Setting/LogicSetting.h"
#include "Setting/GameRuleSetting.h"

namespace BLOCKMAN
{
UUID EntityLivingBase::SPRINTING_SPEED_BOOST_ID = UUID::fromString("662A6B8D-DA3E-4C1C-8813-96EA6097278D");
AttributeModifier* EntityLivingBase::SPRINTING_SPEED_BOOST = NULL;	// = (new AttributeModifier(field_110156_b, "Sprinting speed boost", 0.30000001192092896D, 2)).func_111168_a(false);

void EntityLivingBase::initialize()
{
	SPRINTING_SPEED_BOOST = LordNew AttributeModifier(SPRINTING_SPEED_BOOST_ID, "Sprinting speed boost", 0.3f, 2);
	SPRINTING_SPEED_BOOST->setSaved(false);
}
void EntityLivingBase::unInitialize()
{
	LordSafeDelete(SPRINTING_SPEED_BOOST);
}

EntityLivingBase::EntityLivingBase(World* pWorld)
	: Entity(pWorld)
{
	doConstruct();
}

void EntityLivingBase::init_LivingBase()
{
	combatTracker = LordNew CombatTracker(this);
	attributeMap = LordNew AttributeMap();
	for (size_t i = 0; i < 5; ++i) {
		previousEquipment[i] = nullptr;
	}
	m_attackingPlayerEntityId = 0;
	recentlyHit = 0;
	entityAge = 0;
	prevOnGroundSpeedFactor = 0.f;
	onGroundSpeedFactor = 0.f;
	movedDistance = 0.f;
	prevMovedDistance = 0.f;
	unused180 = 0.f;
	scoreValue = 0;
	lastDamage = 0;
	isJumping = false;
	randomYawVelocity = 0.f;
	newPosRotationIncrements = 0;
	newPos = Vector3::ZERO;
	newRotationYaw = 0.f;
	newRotationPitch = 0.f;
	potionsNeedUpdate = true;
	lastAttacker = NULL;
	lastAttackerTime = 0;
	float landMovementFactor = 0.f;
	jumpTicks = 0;
	absorptionAmount = 0.f;

	isSwingInProgress = false;
	swingProgressInt = 0;
	arrowHitTimer = 0;
	prevHealth = 0.f; 
	hurtTime = 0;
	maxHurtTime = 0;
	attackedAtYaw = 0.f;
	deathTime = 0;
	attackTime = 0;
	prevSwingProgress = 0.f;
	swingProgress = 0.f;
	prevLimbYaw = 0.f;
	limbYaw = 0.f;
	limbSwing = 0.f;
	maxHurtResistantTime = GameRuleSetting::getGameRule()->hurtProtectTime;
	prevCameraPitch = 0.f;
	cameraPitch = 0.f;
	randomUnused2 = 0.f;
	randomUnused1 = 0.f;
	renderYawOffset = 0.f;
	prevRenderYawOffset = 0.f;
	rotationYawHead = 0.f;
	prevRotationYawHead = 0.f;
	jumpMovementFactor = 0.02f;
	moveStrafing = 0.f;
	moveForward = 0.f;
	m_moving = false;
}

EntityLivingBase::~EntityLivingBase()
{
	doDestruct();
}

void EntityLivingBase::doConstruct()
{
	init_LivingBase();
	EntityLivingBase::applyEntityAttributes();
	setEntityHealth(getMaxHealth());
	preventEntitySpawning = true;
	randomUnused1 = (Math::UnitRandom() + 1.0f) * 0.01f;
	setPosition(position);
	randomUnused2 = Math::UnitRandom() * 12398.0f;
	rotationYaw = Math::UnitRandom() * Math::PI * 2.0f;
	rotationYawHead = rotationYaw;
	stepHeight = 0.6F;
}

void EntityLivingBase::doDestruct()
{
	LordSafeDelete(attributeMap);
	LordSafeDelete(combatTracker);
	for (auto p : activePotionsMap) {
		LordSafeDelete(p.second);
	}
	activePotionsMap.clear();
}


void EntityLivingBase::entityInit()
{
	data_Hand_State = 0;
	data_Potion_Effects = 0;
	data_Hide_Particles = 0;
	data_Arrow_Count =0;
	data_Health = 1.0f;
}

void EntityLivingBase::applyEntityAttributes()
{
	attributeMap->registerAttribute(SharedMonsterAttributes::EAMAX_HEALTH);
	attributeMap->registerAttribute(SharedMonsterAttributes::KNOCKBACK_RESISTANCE);
	attributeMap->registerAttribute(SharedMonsterAttributes::MOVEMENT_SPEED);
	attributeMap->registerAttribute(SharedMonsterAttributes::ATTACK_DAMAGE);
	if (!isAIEnabled())
	{
		attributeMap->getAttributeInstance(SharedMonsterAttributes::MOVEMENT_SPEED)->setBaseValue(0.1f);
	}
}

void EntityLivingBase::updateFallState(float y, bool onGroundIn)
{
	if (!isInWater())
	{
		handleWaterMovement();
	}

	if (onGroundIn && fallDistance > 0.0f)
	{
		BlockPos pos = (position - Vector3(0.f, 0.2f + yOffset, 0.f)).getFloor();
		int blockID = world->getBlockId(pos);

		if (blockID == 0)		 {
			int minx = int(Math::Floor(position.x - boundingBox.getDX() / 2));
			int maxx = int(Math::Ceil(position.x + boundingBox.getDX() / 2));
			int minz = int(Math::Floor(position.z - boundingBox.getDZ() / 2));
			int maxz = int(Math::Ceil(position.z + boundingBox.getDZ() / 2));

			for (int x = minx; x <= maxx; ++x)
				for (int z = minz; z <= maxz; ++z) {
					blockID = world->getBlockId(BlockPos(x, pos.y, z));
					if (blockID != 0)
						goto FIND_BLOCK;
				}
		}
FIND_BLOCK:

		if (blockID == 0)
		{
			int blockID2 = world->getBlockId(pos.getNegY());

			if (blockID2 == BLOCK_ID_FENCE ||
				blockID2 == BLOCK_ID_FENCE_GATE ||
				blockID2 == BLOCK_ID_COBBLE_STONE_WALL)
			{
				blockID = blockID2;
			}
		}

		if (blockID > 0)
		{
			BlockManager::sBlocks[blockID]->onFallenUpon(world, pos, this, fallDistance);
		}
	}

	Entity::updateFallState(y, onGroundIn);
}

void EntityLivingBase::onEntityUpdate()
{
	prevSwingProgress = swingProgress;
	prevGunFireProgress = gunFireProgress;
	m_prevReloadPregress = m_reloadPregress;
	Entity::onEntityUpdate();
	
	if (isEntityAlive() && isEntityInsideOpaqueBlock())
	{
		attackEntityFrom(DamageSource::inWall, 1.0F);
	}

	if (isImmuneToFire() || world->m_isClient)
	{
		extinguish();
	}

	EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(this);
	bool invincible = pplayer && (pplayer->capabilities.disableDamage || pplayer->capabilities.isWatchMode);

	if (isEntityAlive() && isInsideOfMaterial(BM_Material::BM_MAT_water))
	{
		if (!canBreatheUnderwater() && !isPotionActive(Potion::waterBreathing->getId()) && !invincible)
		{
			setAir(decreaseAirSupply(getAir()));

			if (getAir() == -20)
			{
				setAir(0);

				for (int i = 0; i < 8; ++i)
				{
					float rx = rand->nextFloat() - rand->nextFloat();
					float ry = rand->nextFloat() - rand->nextFloat();
					float rz = rand->nextFloat() - rand->nextFloat();
					world->spawnParticle("bubble", position + Vector3(rx, ry, rz), motion);
				}
				attackEntityFrom(DamageSource::drown, 2.0F);
			}
		}

		extinguish();

		if (!world->m_isClient && isRiding() /*&& ridingEntity->isClass(ENTITY_CLASS_LIVING)*/)
		{
			mountEntity(NULL);
		}
	}
	else
	{
		setAir(300);
	}

	prevCameraPitch = cameraPitch;

	if (attackTime > 0)
		--attackTime;

	if (hurtTime > 0)
		--hurtTime;

	if (hurtResistantTime > 0)
		--hurtResistantTime;

	if (getHealth() <= 0.0F)
		onDeathUpdate();

	if (recentlyHit > 0)
		--recentlyHit;
	else
		m_attackingPlayerEntityId = 0;

	if (lastAttacker && !lastAttacker->isEntityAlive())
		lastAttacker = NULL;

	updatePotionEffects();
	prevMovedDistance = movedDistance;
	prevRenderYawOffset = renderYawOffset;
	prevRotationYawHead = rotationYawHead;
	prevRotationYaw = rotationYaw;
	prevRotationPitch = rotationPitch;
}

void EntityLivingBase::onDeathUpdate()
{
	++deathTime;

	if (deathTime == 20)
	{
		if (!world->m_isClient && (recentlyHit > 0 || isPlayer()) && !isChild() /*&& world->getGameRules().getGameRuleBooleanValue("doMobLoot")*/)
		{
			EntityPlayer* attackingPlayer = world->getPlayerEntity(m_attackingPlayerEntityId);
			int totalExp = getExperiencePoints(attackingPlayer);

			while (totalExp > 0)
			{
				int exp = EntityXPOrb::getXPSplit(totalExp);
				totalExp -= exp;
				world->spawnEntityInWorld(LordNew EntityXPOrb(world, position, exp));
			}
		}

		setDead();

		for (int i = 0; i < 20; ++i)
		{
			float var8 = float(rand->nextGaussian()) * 0.02f;
			float var4 = float(rand->nextGaussian()) * 0.02f;
			float var6 = float(rand->nextGaussian()) * 0.02f;
			// worldObj.spawnParticle("explode", posX + (double)(rand.nextFloat() * width * 2.0F) - (double)width, posY + (double)(rand.nextFloat() * height), posZ + (double)(rand.nextFloat() * width * 2.0F) - (double)width, var8, var4, var6);
		}
	}
}

int EntityLivingBase::decreaseAirSupply(int air)
{
	int var2 = EnchantmentHelper::getRespiration(this);
	return var2 > 0 && rand->nextInt(var2 + 1) > 0 ? air : air - 1;
}

void EntityLivingBase::setLastAttacker(Entity* pEntity)
{
	EntityLivingBase* pliving = dynamic_cast<EntityLivingBase*>(pEntity);
	if (pliving)
	{
		lastAttacker = pliving;
	}
	else
	{
		lastAttacker = NULL;
	}

	lastAttackerTime = ticksExisted;
}

void EntityLivingBase::writeEntityToNBT(NBTTagCompound* pNBT)
{
	pNBT->setFloat("HealF", getHealth());
	pNBT->setShort("Health", (short)((int)Math::Ceil(getHealth())));
	pNBT->setShort("HurtTime", (short)hurtTime);
	pNBT->setShort("DeathTime", (short)deathTime);
	pNBT->setShort("AttackTime", (short)attackTime);
	pNBT->setFloat("AbsorptionAmount", getAbsorptionAmount());
	ItemStackArr items = getLastActiveItems();
	int itemsCount = getLastActiveItemsCount();
	for (int i = 0; i < itemsCount; ++i)
	{
		ItemStackPtr pStack = items[i];

		if (pStack)
		{
			AttributeModifierMap modifiers;
			pStack->getAttributeModifiers(modifiers);
			attributeMap->removeAttributeModifiers(modifiers);
		}
	}
	pNBT->setTag("Attributes", SharedMonsterAttributes::writeBaseAttributeMapToNBT(attributeMap));
	for (int i = 0; i < itemsCount; ++i)
	{
		ItemStackPtr pStack = items[i];

		if (pStack)
		{
			AttributeModifierMap modifiers;
			pStack->getAttributeModifiers(modifiers);
			attributeMap->applyAttributeModifiers(modifiers);
		}
	}

	NBTTagList* nbtPotion = NULL;
	if (!activePotionsMap.empty())
		nbtPotion = LordNew NBTTagList();

	for (ActivePotionsMap::iterator it = activePotionsMap.begin(); it != activePotionsMap.end(); ++it)
	{
		PotionEffect* effect = it->second;
		nbtPotion->appendTag(effect->writeCustomPotionEffectToNBT(LordNew NBTTagCompound()));
	}
	if (nbtPotion)
		pNBT->setTag("ActiveEffects", nbtPotion);
}

void EntityLivingBase::readEntityFromNBT(NBTTagCompound* pNBT)
{
	setAbsorptionAmount(pNBT->getFloat("AbsorptionAmount"));

	if (pNBT->hasKey("Attributes") && world != NULL && !world->m_isClient)
	{
		SharedMonsterAttributes::setAttributeModifiers(attributeMap, pNBT->getTagList("Attributes"));
	}

	if (pNBT->hasKey("ActiveEffects"))
	{
		NBTTagList* nbtEffects = pNBT->getTagList("ActiveEffects");

		for (int i = 0; i < nbtEffects->tagCount(); ++i)
		{
			NBTTagCompound* nbtEffect = (NBTTagCompound*)nbtEffects->tagAt(i);
			PotionEffect* pEffect = PotionEffect::readCustomPotionEffectFromNBT(nbtEffect);
			activePotionsMap.insert(std::make_pair(pEffect->getPotionID(), pEffect));
		}
	}

	if (pNBT->hasKey("HealF"))
	{
		setEntityHealth(pNBT->getFloat("HealF"));
	}
	else
	{
		NBTBase* pbase = pNBT->getTag("Health");

		if (pbase == NULL)
		{
			setEntityHealth(getMaxHealth());
		}
		else if (pbase->getId() == 5)
		{
			setEntityHealth(((NBTTagFloat*)pbase)->data);
		}
		else if (pbase->getId() == 2)
		{
			setEntityHealth((float)((NBTTagShort*)pbase)->data);
		}
	}

	hurtTime = pNBT->getShort("HurtTime");
	deathTime = pNBT->getShort("DeathTime");
	attackTime = pNBT->getShort("AttackTime");
}

void EntityLivingBase::updatePotionEffects()
{
	for (ActivePotionsMap::iterator it = activePotionsMap.begin(); it != activePotionsMap.end(); )
	{
		int poitionID = it->first;
		PotionEffect* effect = it->second;
		if (!effect->onUpdate(this))
		{
			if (!world->m_isClient)
			{
				it = activePotionsMap.erase(it);
				onFinishedPotionEffect(effect);
				LordDelete(effect);
				continue;
			}
		}
		/*else if(effect->getDuration() % 600 == 0)
		{
			onChangedPotionEffect(effect, false);
		}*/
		++it;
	}

	if (potionsNeedUpdate)
	{
		if (world->m_isClient)
		{
			updatePotionMetadata();
		}

		potionsNeedUpdate = false;
	}

	int effects = data_Potion_Effects;
	bool hidecpas = data_Hide_Particles;

	if (effects > 0)
	{
		bool flag = false;

		if (!isInvisible())
		{
			flag = rand->nextBool();
		}
		else
		{
			flag = rand->nextInt(15) == 0;
		}

		if (hidecpas)
		{
			flag &= rand->nextInt(5) == 0;
		}

		if (flag && effects > 0)
		{
			float r = (effects >> 16 & 255) / 255.f;
			float g = (effects >> 8 & 255) / 255.f;
			float b = (effects >> 0 & 255) / 255.f;
			Vector3 loc;
			loc.x = position.x + (rand->nextFloat() - 0.5f) * width;
			loc.y = position.y + rand->nextFloat()*height - yOffset;
			loc.z = position.z + (rand->nextFloat() - 0.5f) * width;

			world->spawnParticle(hidecpas ? "mobSpellAmbient" : "mobSpell", loc, Vector3(r, g, b));
		}
	}
}

void EntityLivingBase::updatePotionMetadata()
{
	if (world->m_isClient)
	{
		if (activePotionsMap.empty())
		{
			data_Hide_Particles = false;
			data_Potion_Effects = 0;
			setInvisible(false);
		}
		else
		{
			PotionEffectArr potions;
			for (auto p : activePotionsMap)
				potions.push_back(p.second);
			int color = PotionHelper::calcPotionLiquidColor(potions); // get all the potion effects color.
			data_Hide_Particles = PotionHelper::isAmbient(potions);
			data_Potion_Effects = color;
			setInvisible(isPotionActive(Potion::invisibility->getId()));
		}
	}
}

void EntityLivingBase::clearActivePotions()
{
	for (ActivePotionsMap::iterator it = activePotionsMap.begin(); it != activePotionsMap.end(); )
	{
		int potionID = it->first;
		PotionEffect* effect = it->second;
		it = activePotionsMap.erase(it);
		onFinishedPotionEffect(effect);
		LordDelete(effect);
	}
}

bool EntityLivingBase::isPotionActive(Potion* poition)
{
	return activePotionsMap.find(poition->getId()) != activePotionsMap.end();
}

PotionEffect* EntityLivingBase::getActivePotionEffect(Potion* potion)
{
	ActivePotionsMap::iterator it = activePotionsMap.find(potion->getId());
	if (it != activePotionsMap.end())
	{
		return it->second;
	}
	return NULL;
}

void EntityLivingBase::addPotionEffect(PotionEffect* pEffect)
{
	if (!isPotionApplicable(pEffect))
		return;

	ActivePotionsMap::iterator it = activePotionsMap.find(pEffect->getPotionID());
	if (it != activePotionsMap.end())
	{
		it->second->combine(pEffect);
		onChangedPotionEffect(it->second, true);
	}
	else
	{
		activePotionsMap.insert(std::make_pair(pEffect->getPotionID(), pEffect));
		onNewPotionEffect(pEffect);
	}
}

bool EntityLivingBase::isPotionApplicable(PotionEffect* pEffect)
{
	if (getCreatureAttribute() == CREATURE_ATTRIBUTE_UNDEAD)
	{
		int potionID = pEffect->getPotionID();

		if (potionID == Potion::regeneration->getId()
			|| potionID == Potion::poison->getId())
		{
			return false;
		}
	}

	return true;
}

void EntityLivingBase::removePotionEffect(int potionID)
{
	ActivePotionsMap::iterator it = activePotionsMap.find(potionID);
	if (it == activePotionsMap.end())
		return;

	PotionEffect* pEffect = it->second;
	activePotionsMap.erase(it);

	if (pEffect != NULL)
	{
		onFinishedPotionEffect(pEffect);
	}
	LordDelete(pEffect);
}

void EntityLivingBase::onNewPotionEffect(PotionEffect* pEffect)
{
	potionsNeedUpdate = true;

	/*if (!world->m_isClient)*/
	{
		Potion::potionTypes[pEffect->getPotionID()]->applyAttributesModifiersToEntity(this, attributeMap, pEffect->getAmplifier());
	}
}

void EntityLivingBase::onChangedPotionEffect(PotionEffect* pEffect, bool apply)
{
	potionsNeedUpdate = true;

	if (apply/* && !world->m_isClient*/)
	{
		Potion::potionTypes[pEffect->getPotionID()]->removeAttributesModifiersFromEntity(this, attributeMap, pEffect->getAmplifier());
		Potion::potionTypes[pEffect->getPotionID()]->applyAttributesModifiersToEntity(this, attributeMap, pEffect->getAmplifier());
	}
}

void EntityLivingBase::onFinishedPotionEffect(PotionEffect* pEffect)
{
	potionsNeedUpdate = true;

	/*if (!world->m_isClient)*/
	{
		Potion::potionTypes[pEffect->getPotionID()]->removeAttributesModifiersFromEntity(this, attributeMap, pEffect->getAmplifier());
	}
}

void EntityLivingBase::heal(float amount)
{
	float currentHeal = getHealth();

	if (currentHeal > 0.0F)
	{
		setEntityHealth(currentHeal + amount);
	}
}

void EntityLivingBase::setEntityHealth(float amount)
{
	float preHelth = getHealth();
	data_Health = Math::Clamp(amount, 0.f, getMaxHealth());
	if (data_Health < preHelth)
	{
		hurtTime = maxHurtTime = 10;
		if (data_Health > 0)
		{
			limbYaw = 1.5F;
			hurtResistantTime = maxHurtResistantTime;
			hurtTime = maxHurtTime = 10;
			attackedAtYaw = 0.0F;
			//playSound(getHurtSound(), getSoundVolume(), (rand->nextFloat() - rand->nextFloat()) * 0.2F + 1.0F);
			playSoundByType(getHurtSoundType());
			attackEntityFrom(DamageSource::generic, 0.0F);
		}
		else
		{
			//playSound(getDeathSound(), getSoundVolume(), (rand->nextFloat() - rand->nextFloat()) * 0.2F + 1.0F);
			//playSoundByType(getDeathSoundType());
			setEntityHealth(0.0F);
			onDeath(DamageSource::generic);
		}
	}
}

void EntityLivingBase::setEntityMaxHealth(float maxHealth)
{
	getEntityAttribute(SharedMonsterAttributes::EAMAX_HEALTH)->setBaseValue(maxHealth);
}

bool EntityLivingBase::attackEntityFrom(DamageSource* pSource, float amount)
{
	if (isEntityInvulnerable())
	{
		return false;
	}
	else if (world->m_isClient)
	{
		return false;
	}
	else
	{
		entityAge = 0;

		if (getHealth() <= 0.0F)
		{
			return false;
		}
		else if (pSource->isFireDamage() && isPotionActive(Potion::fireResistance))
		{
			return false;
		}
		else
		{
			if ((pSource == DamageSource::anvil || pSource == DamageSource::fallingBlock) && getCurrentItemOrArmor(4) != NULL)
			{
				getCurrentItemOrArmor(4)->damageItem((int)(amount * 4.0F + rand->nextFloat() * amount * 2.0F), this);
				amount *= 0.75F;
			}

			Entity* attacker = pSource->getEntity();

			if (attacker != NULL)
			{
				EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(attacker);
				if (pPlayer)
				{
					recentlyHit = 100;
					m_attackingPlayerEntityId = pPlayer->entityId;
				}
				// todo.
				/*else if (attacker->isClass(ENTITY_CLASS_WOLF))
				{
				EntityWolf var5 = (EntityWolf)attacker;

				if (var5.isTamed())
				{
				recentlyHit = 100;
				attackingPlayer = null;
				}
				}*/
			}

			limbYaw = 1.5F;
			bool flag = true;

			if (hurtResistantTime > 0)
			{
				if (amount <= lastDamage)
				{
					return false;
				}

				damageEntity(pSource, amount - lastDamage);
				lastDamage = amount;
				flag = false;
			}
			else
			{
				lastDamage = amount;
				prevHealth = getHealth();
				hurtResistantTime = maxHurtResistantTime;
				damageEntity(pSource, amount);
				hurtTime = maxHurtTime = 10;
			}

			attackedAtYaw = 0.0F;

			if (flag)
			{
				world->setEntityState(this, (i8)2);

				if (pSource != DamageSource::drown)
				{
					setBeenAttacked();
				}

				if (attacker != NULL)
				{
					float deltax = attacker->position.x - position.x;
					float deltaz = attacker->position.z - position.z;

					for (; deltax * deltax + deltaz * deltaz < 1e-4f; deltaz = (Math::UnitRandom() - Math::UnitRandom()) * 0.01f)
					{
						deltax = (Math::UnitRandom() - Math::UnitRandom()) * 0.01f;
					}

					attackedAtYaw = (Math::ATan2(deltaz, deltax) * 180.0f / Math::PI) - rotationYaw;
					//knockBack(attacker, amount, deltax, deltaz);
					knockBack(attacker, pSource->knockBackDis * m_knockBackCoefficient, deltax, deltaz);
				}
				else
				{
					attackedAtYaw = (int)(Math::UnitRandom() * 2.0f) * 180.f;
				}
			}

			if (getHealth() <= 0.0f)
			{
				if (flag)
				{
					//playSound(getDeathSound(), getSoundVolume(), getSoundPitch());
					//playSoundByType(getDeathSoundType());
				}
			}
			else if (flag)
			{
				//playSound(getHurtSound(), getSoundVolume(), getSoundPitch());
				playSoundByType(getHurtSoundType());
			}

			return true;
		}
	}
}

void EntityLivingBase::renderBrokenItemStack(ItemStackPtr par1ItemStack)
{
	//playSound("random.break", 0.8F, 0.8F + world->m_Rand.nextFloat() * 0.4F);
	playSoundByType(ST_Break);

	//for (int var2 = 0; var2 < 5; ++var2)
	//{
	//	Vec3 var3 = worldObj.getWorldVec3Pool().getVecFromPool(((double)rand.nextFloat() - 0.5D) * 0.1D, Math.random() * 0.1D + 0.1D, 0.0D);
	//	var3.rotateAroundX(-rotationPitch * (float)Math.PI / 180.0F);
	//	var3.rotateAroundY(-rotationYaw * (float)Math.PI / 180.0F);
	//	Vec3 var4 = worldObj.getWorldVec3Pool().getVecFromPool(((double)rand.nextFloat() - 0.5D) * 0.3D, (double)(-rand.nextFloat()) * 0.6D - 0.3D, 0.6D);
	//	var4.rotateAroundX(-rotationPitch * (float)Math.PI / 180.0F);
	//	var4.rotateAroundY(-rotationYaw * (float)Math.PI / 180.0F);
	//	var4 = var4.addVector(posX, posY + (double)getEyeHeight(), posZ);
	//	worldObj.spawnParticle("iconcrack_" + par1ItemStack.getItem().itemID, var4.xCoord, var4.yCoord, var4.zCoord, var3.xCoord, var3.yCoord + 0.05D, var3.zCoord);
	//}
}

void EntityLivingBase::onDeath(DamageSource* pSource)
{
	Entity* pEntity = pSource->getEntity();
	EntityLivingBase* attacker = getAttackingEntity();

	if (scoreValue >= 0 && attacker != NULL)
	{
		attacker->addToPlayerScore(this, scoreValue);
	}

	if (pEntity != NULL)
	{
		pEntity->onKillEntity(this);
	}

	setDead();

	if (!world->m_isClient)
	{
		int i = 0;

		EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(pEntity);
		if (pplayer)
		{
			// i = EnchantmentHelper.getLootingModifier((EntityLivingBase)pEntity);
		}

		if (!isChild()/* && worldObj.getGameRules().getGameRuleBooleanValue("doMobLoot")*/)
		{
			dropFewItems(recentlyHit > 0, i);
			dropEquipment(recentlyHit > 0, i);

			if (recentlyHit > 0)
			{
				int var5 = rand->nextInt(200) - i;

				if (var5 < 5)
				{
					dropRareDrop(var5 <= 0 ? 1 : 0);
				}
			}
		}
	}

	world->setEntityState(this, (i8)3);
}


//real logic has move to EntityPlayerMP in server module
//
void EntityLivingBase::knockBack(Entity* pEntity, float strength, float xRatio, float zRatio)
{

}


bool EntityLivingBase::isOnLadder()
{
	BlockPos pos = Vector3(position.x, boundingBox.vMin.y, position.z).getFloor();
	int blockID = world->getBlockId(pos);
	return blockID == BLOCK_ID_LADDER || blockID == BLOCK_ID_VINE;
}

void EntityLivingBase::fall(float distance)
{
	Entity::fall(distance);
	PotionEffect* jumpEffect = getActivePotionEffect(Potion::jump);
	float jumpAmplifier = jumpEffect != NULL ? (float)(jumpEffect->getAmplifier() + 1) : 0.0F;
	int distance1 = int(Math::Ceil(distance - 3.0F - jumpAmplifier));

	if (distance1 >= -1)
	{
		if (distance1 > 3)
			playSoundByType(ST_FallBig);
		else
			playSoundByType(ST_FallSmall);
	}

	if (distance1 > 0)
	{
		BlockPos pos = (position - Vector3(0.f, 0.2f + yOffset, 0.f)).getFloor();
		int blockID = world->getBlockId(pos);
		
		if (blockID == 0) {
			int minx = int(Math::Floor(position.x - boundingBox.getDX() / 2));
			int maxx = int(Math::Ceil(position.x + boundingBox.getDX() / 2));
			int minz = int(Math::Floor(position.z - boundingBox.getDZ() / 2));
			int maxz = int(Math::Ceil(position.z + boundingBox.getDZ() / 2));

			for (int x = minx; x <= maxx; ++x)
				for (int z = minz; z <= maxz; ++z) {
					blockID = world->getBlockId(BlockPos(x, pos.y, z));
					if (blockID != 0)
						goto FIND_BLOCK;
				}
		}
			
FIND_BLOCK:
		if (!m_flyingByPulled && (blockID <= 0 || !BlockManager::sBlocks[blockID]->immune_fall_damage())) {
			attackEntityFrom(DamageSource::fall, (float)distance1);
		}
	}
}

int EntityLivingBase::getTotalArmorValue()
{
	int result = 0;
	ItemStackArr stacks = getLastActiveItems();
	int itemscount = getLastActiveItemsCount();

	for (int i = 0; i < itemscount; ++i)
	{
		if (!stacks[i])
			continue;

		ItemArmor* pArmor = dynamic_cast<ItemArmor*>(stacks[i]->getItem());
		if (pArmor)
		{
			result += pArmor->damageReduceAmount;
		}
	}

	return result;
}

float EntityLivingBase::applyArmorCalculations(DamageSource* pSource, float damage)
{
	if (!pSource->isUnblockable())
	{
		damageArmor(damage);
		return damage - damage * getTotalArmorValue() * LogicSetting::Instance()->getAttackCoefficientX();
	}

	return damage;
}

float EntityLivingBase::applyPotionDamageCalculations(DamageSource* pSource, float damage)
{
	int var3;
	int var4;
	float var5;

	if (isPotionActive(Potion::resistance) && pSource != DamageSource::outOfWorld)
	{
		var3 = (getActivePotionEffect(Potion::resistance)->getAmplifier() + 1) * 5;
		var4 = 25 - var3;
		var5 = damage * (float)var4;
		damage = var5 / 25.0F;
	}

	if (damage <= 0.0F)
	{
		return 0.0F;
	}
	else
	{
		var3 = EnchantmentHelper::getEnchantmentModifierDamage(getLastActiveItems(), pSource);

		if (var3 > 20)
		{
			var3 = 20;
		}

		if (var3 > 0 && var3 <= 20)
		{
			var4 = 25 - var3;
			var5 = damage * (float)var4;
			damage = var5 / 25.0F;
		}

		return damage;
	}
}

void EntityLivingBase::damageEntity(DamageSource* pSource, float damage)
{
	if (isEntityInvulnerable())
		return;

	damage = applyArmorCalculations(pSource, damage);
	damage = applyPotionDamageCalculations(pSource, damage);
	float d = damage;
	damage = Math::Max(damage - getAbsorptionAmount(), 0.0F);
	setAbsorptionAmount(getAbsorptionAmount() - (d - damage));

	if (damage != 0.0F)
	{
		float hp = getHealth();
		setEntityHealth(hp - damage);
		if (damage >= hp && damage >= 15)
		{
			LordLogInfo("====setEntityHealth====EntityLivingBase::damageEntity current hp %f, damage %f DamageSource %s",
				hp, damage, pSource->damageType.c_str());
		}
		/*
		comment by wanglei
		reason: not used now and cause server crash because entity in damageSource destroyed but still referece
		combatTracker->trackDamage(pSource, hp, damage);
		*/
		
		setAbsorptionAmount(getAbsorptionAmount() - damage);
	}
}

EntityLivingBase* EntityLivingBase::getAttackingEntity()
{
	EntityLivingBase* attacker = combatTracker->getBestAttacker(); // Note that attacker is always null because there is nothing in combatTracker, comment by oujie
	if (attacker)
	{
		return attacker;
	}
	else if (m_attackingPlayerEntityId > 0)
	{
		EntityPlayer* attackingPlayer = world->getPlayerEntity(m_attackingPlayerEntityId);
		return attackingPlayer;
	}
		
	return NULL;
}

int EntityLivingBase::getArmSwingAnimationEnd()
{
	int result = 6;
	if (isPotionActive(Potion::digSpeed))
	{
		result = 6 - (1 + getActivePotionEffect(Potion::digSpeed)->getAmplifier()) * 1;
	}
	else if(isPotionActive(Potion::digSlowdown))
	{
		result = 6 + (1 + getActivePotionEffect(Potion::digSlowdown)->getAmplifier()) * 2;
	}
	return result;
}

int EntityLivingBase::getArmHoldingGunAnimEnd()
{
	int result = 9;
	// it determine by current gun's shooting frequency.
	return result;
}

void EntityLivingBase::swingItem()
{
	if (isHoldGun)
	{
		if ((!m_isReloadBullet) &&
			(!isHoldGunFire || gunFireInt >= getArmHoldingGunAnimEnd() / 2 || gunFireInt < 0))
		{
			gunFireInt = -1;
			isHoldGunFire = true;

			// todo.
			/*if (worldObj instanceof WorldServer)
			{
			((WorldServer)worldObj).getEntityTracker().sendPacketToAllPlayersTrackingEntity(this, new Packet18Animation(this, 1));
			}*/
		}
	}
	else
	{
		if (!isSwingInProgress || swingProgressInt >= getArmSwingAnimationEnd() / 2 || swingProgressInt < 0)
		{
			swingProgressInt = -1;
			isSwingInProgress = true;

			// todo.
			/*if (worldObj instanceof WorldServer)
			{
				((WorldServer)worldObj).getEntityTracker().sendPacketToAllPlayersTrackingEntity(this, new Packet18Animation(this, 1));
			}*/
		}
	}
}

void EntityLivingBase::kill()
{
	attackEntityFrom(DamageSource::outOfWorld, 4.0F);
}

void EntityLivingBase::updateArmSwingProgress()
{
	if (isHoldGun)
	{
		int i = getArmHoldingGunAnimEnd();
		if (isHoldGunFire)
		{
			++gunFireInt;

			if (gunFireInt >= i)
			{
				gunFireInt = 0;
				isHoldGunFire = false;
			}
		}
		else
		{
			gunFireInt = 0;
		}
		gunFireProgress = (float)gunFireInt / (float)i;
	}
	else
	{

		int i = getArmSwingAnimationEnd();
		if (isSwingInProgress)
		{
			++swingProgressInt;

			if (swingProgressInt >= i)
			{
				swingProgressInt = 0;
				isSwingInProgress = false;
			}
		}
		else
		{
			swingProgressInt = 0;
		}
		swingProgress = (float)swingProgressInt / (float)i;
	}
}

void EntityLivingBase::setSprinting(bool sprinting)
{
	Entity::setSprinting(sprinting);
	IAttributeInstance* iattributeinstance = getEntityAttribute(SharedMonsterAttributes::MOVEMENT_SPEED);

	if (iattributeinstance->getModifier(SPRINTING_SPEED_BOOST_ID) != NULL)
		iattributeinstance->removeModifier(SPRINTING_SPEED_BOOST);

	if (sprinting)
		iattributeinstance->applyModifier(SPRINTING_SPEED_BOOST);
}

float EntityLivingBase::getSoundPitch()
{
	float result = 0.f;
	if (isChild())
		result = (rand->nextFloat() - rand->nextFloat()) * 0.2F + 1.5F;
	else
		result = (rand->nextFloat() - rand->nextFloat()) * 0.2F + 1.0F;
	return result;
}

void EntityLivingBase::dismountEntity(Entity* pEntity)
{
	float x = pEntity->position.x;
	float y = pEntity->boundingBox.vMin.y + pEntity->height;
	float z = pEntity->position.z;

	for (float fx = -1.5f; fx < 2.0f; ++fx)
	{
		for (float fz = -1.5f; fz < 2.0f; ++fz)
		{
			if (fx != 0.0f || fz != 0.0f)
			{
				int ix = (int)(position.x + fx);
				int iz = (int)(position.z + fz);
				Box aabb = boundingBox.getOffset(fx, 1.0f, fz);

				if (world->getCollidingBlockBounds(aabb).empty())
				{
					BlockPos pos(ix, (int)position.y, iz);
					if (world->doesBlockHaveSolidTopSurface(pos))
					{
						setPositionAndUpdate(position + Vector3(fx, 1.f, fz));
						return;
					}

					if (world->doesBlockHaveSolidTopSurface(pos.getNegY()) || world->getBlockMaterial(pos.getNegY()) == BM_Material::BM_MAT_water)
					{
						x = position.x + fx;
						y = position.y + 1.0f;
						z = position.z + fz;
					}
				}
			}
		}
	}

	setPositionAndUpdate(Vector3(x, y, z));
}

void EntityLivingBase::jump()
{
	motion.y = 0.42f;

	if (isPotionActive(Potion::jump))
	{
		motion.y += (float)(getActivePotionEffect(Potion::jump)->getAmplifier() + 1) * 0.1f;
	}

	if (isSprinting())
	{
		float var1 = rotationYaw * 0.017453292F;
		motion.x -= Math::Sin(var1) * 0.2F;
		motion.z += Math::Cos(var1) * 0.2F;
	}

	isAirBorne = true;
}

void EntityLivingBase::moveEntityWithHeading(float strafe, float forward)
{
	EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(this);
	float dt = world->getFrameDeltaTime();
	if (isInWater() && (!pplayer || (!pplayer->capabilities.isFlying)))
	{
		moveEntityInWater(strafe, forward, dt);
	}
	else if (handleLavaMovement() && (!pplayer || (!pplayer->capabilities.isFlying)))
	{
		moveEntityInLava(strafe, forward, dt);
	}
	else if (pplayer && pplayer->capabilities.isParachute)
	{
		moveEntityWhenParachute(strafe, forward, dt);
	}
	else if (this->isFlyEntity())
	{
		moveEntityFly(strafe, forward, dt);
	}
	else
	{
		moveEntityCommon(strafe, forward, dt);
	}

	prevLimbYaw = limbYaw;
	float deltax = position.x - prevPos.x;
	float deltaz = position.z - prevPos.z;
	m_moving = (deltax * deltax + deltaz * deltaz) > 0.0004f;
	float delta = Math::Sqrt(deltax * deltax + deltaz * deltaz) * 4.0F;

	if (delta > 1.0F)
	{
		delta = 1.0F;
	}

	limbYaw += (delta - limbYaw) * 0.4F;
	limbSwing += limbYaw;
}

bool EntityLivingBase::attackEntityAsMob(Entity* pEntity)
{
	setLastAttacker(pEntity);
	return false;
}

void EntityLivingBase::onUpdate()
{
	Entity::onUpdate();

	if (!world->m_isClient)
	{
		int arrowCount = getArrowCountInEntity();

		if (arrowCount > 0)
		{
			if (arrowHitTimer <= 0)
			{
				arrowHitTimer = 20 * (30 - arrowCount);
			}

			--arrowHitTimer;

			if (arrowHitTimer <= 0)
			{
				setArrowCountInEntity(arrowCount - 1);
			}
		}

		for (int i = 0; i < 5; ++i)
		{
			ItemStackPtr previous = previousEquipment[i];
			ItemStackPtr current = getCurrentItemOrArmor(i);

			// todo.
			/*if (!ItemStack::areItemStacksEqual(current, previous))
			{
				((WorldServer)worldObj).getEntityTracker().sendPacketToAllPlayersTrackingEntity(this, new Packet5PlayerInventory(entityId, i, current));

				if (previous != null)
				{
					field_110155_d.func_111148_a(previous.func_111283_C());
				}

				if (current != null)
				{
					field_110155_d.func_111147_b(current.func_111283_C());
				}

				previousEquipment[i] = current == null ? null : current.copy();
			}*/
		}
	}

	onLivingUpdate();
	float deltax = position.x - prevPos.x;
	float deltaz = position.z - prevPos.z;
	float lensqr = (float)(deltax * deltax + deltaz * deltaz);
	float yawoffset = renderYawOffset;
	float len = 0.0F;
	prevOnGroundSpeedFactor = onGroundSpeedFactor;
	float speed = 0.0F;

	if (lensqr > 0.0025f)
	{
		speed = 1.0F;
		len = Math::Sqrt(lensqr) * 3.0F;
		yawoffset = Math::ATan2(deltaz, deltax) * 180.0f / Math::PI - 90.0F;
	}

	if (swingProgress > 0.0F)
		yawoffset = rotationYaw;

	if (!onGround)
		speed = 0.0F;

	onGroundSpeedFactor += (speed - onGroundSpeedFactor) * 0.3F;
	len = updateDistance(yawoffset, len); // updateDistance
	
	while (rotationYaw - prevRotationYaw < -180.0F)
		prevRotationYaw -= 360.0F;

	while (rotationYaw - prevRotationYaw >= 180.0F)
		prevRotationYaw += 360.0F;

	while (renderYawOffset - prevRenderYawOffset < -180.0F)
		prevRenderYawOffset -= 360.0F;

	while (renderYawOffset - prevRenderYawOffset >= 180.0F)
		prevRenderYawOffset += 360.0F;

	while (rotationPitch - prevRotationPitch < -180.0F)
		prevRotationPitch -= 360.0F;

	while (rotationPitch - prevRotationPitch >= 180.0F)
		prevRotationPitch += 360.0F;

	while (rotationYawHead - prevRotationYawHead < -180.0F)
		prevRotationYawHead -= 360.0F;

	while (rotationYawHead - prevRotationYawHead >= 180.0F)
		prevRotationYawHead += 360.0F;

	movedDistance += len;
}

float EntityLivingBase::updateDistance(float yaw, float distance)
{
	float f1 = Math::WrapDegree(yaw - renderYawOffset);
	renderYawOffset += f1 * 0.3F;
	float f2 = Math::WrapDegree(rotationYaw - renderYawOffset);
	bool flag = f2 < -90.0f || f2 >= 90.0f;

	if (f2 < -75.0f)
		f2 = -75.0f;

	if (f2 >= 75.0f)
		f2 = 75.0f;

	renderYawOffset = rotationYaw - f2;

	if (f2 * f2 > 2500.0f)
	{
		renderYawOffset += f2 * 0.2f;
	}

	if (flag)
	{
		distance *= -1.0F;
	}

	return distance;
}

void EntityLivingBase::onLivingUpdate()
{
	if (jumpTicks > 0)
	{
		--jumpTicks;
	}

	if (newPosRotationIncrements > 0)
	{
		Vector3 pos = position + (newPos - position) / float(newPosRotationIncrements);
		float deltaYaw = newRotationYaw - rotationYaw;
		deltaYaw = Math::Mod(deltaYaw, 360.f);
		if (deltaYaw > 180.f)
			deltaYaw -= 360.f;
		if (deltaYaw < -180.f)
			deltaYaw += 360.f;

		rotationYaw = rotationYaw + deltaYaw / newPosRotationIncrements;
		rotationPitch = rotationPitch + (newRotationPitch - rotationPitch) / newPosRotationIncrements;
		--newPosRotationIncrements;
		setPosition(pos);
		setRotation(rotationYaw, rotationPitch);
	}
	//else if (!isClientWorld())
	else
	{
		motion *= 0.98f;
	}

	if (Math::Abs(motion.x) < 0.005f)
		motion.x = 0.0f;
	if (Math::Abs(motion.y) < 0.005f)
		motion.y = 0.0f;
	if (Math::Abs(motion.z) < 0.005f)
		motion.z = 0.0f;

	if (isMovementBlocked())
	{
		isJumping = false;
		moveStrafing = 0.0F;
		moveForward = 0.0F;
		randomYawVelocity = 0.0F;
	}
	else
	{
		updateEntityActionState();
		rotationYawHead = rotationYaw;
	}

	if (isJumping)
	{
		if (!isInWater() && !handleLavaMovement())
		{
			if (onGround && jumpTicks == 0)
			{
				jump();
				jumpTicks = 10;
			}
		}
		else
		{
			motion.y += 0.04f;
		}
	}
	else
	{
		jumpTicks = 0;
	}

	moveStrafing *= 0.98F;
	moveForward *= 0.98F;
	randomYawVelocity *= 0.9F;
	moveEntityWithHeading(moveStrafing, moveForward);

	if (!world->m_isClient)
	{
		collideWithNearbyEntities();
	}
}

void EntityLivingBase::collideWithNearbyEntities()
{
	EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.expland(0.2f, 0.0f, 0.2f));

	for (int i = 0; i < int(entities.size()); ++i)
	{
		Entity* entity = entities[i];

		if (entity->canBeCollidedWith())
		{
			collideWithEntity(entity);
		}
	}
}

void EntityLivingBase::updateRidden()
{
	Entity::updateRidden();
	prevOnGroundSpeedFactor = onGroundSpeedFactor;
	onGroundSpeedFactor = 0.0F;
	fallDistance = 0.0F;
}

void EntityLivingBase::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
{
	yOffset = 0.0F;
	newPos = pos;
	newRotationYaw = yaw;
	newRotationPitch = pitch;
	newPosRotationIncrements = increment;
}

void EntityLivingBase::onItemPickup(Entity* pickedEntity, int stackSize)
{
	if (!pickedEntity->isDead && !world->m_isClient)
	{
		// todo.
		/*EntityTracker var3 = ((WorldServer)worldObj).getEntityTracker();

		if (pickedEntity instanceof EntityItem)
		{
			var3.sendPacketToAllPlayersTrackingEntity(pickedEntity, new Packet22Collect(pickedEntity.entityId, entityId));
		}

		if (pickedEntity instanceof EntityArrow)
		{
			var3.sendPacketToAllPlayersTrackingEntity(pickedEntity, new Packet22Collect(pickedEntity.entityId, entityId));
		}

		if (pickedEntity instanceof EntityXPOrb)
		{
			var3.sendPacketToAllPlayersTrackingEntity(pickedEntity, new Packet22Collect(pickedEntity.entityId, entityId));
		}*/
	}
}

bool EntityLivingBase::canEntityBeSeen(Entity* pEntity)
{
	Vector3 start(position.x, position.y + getEyeHeight(), position.z);
	Vector3 end(pEntity->position.x, pEntity->position.y + pEntity->getEyeHeight(), pEntity->position.z);
	return !world->rayTraceBlocks(start, end).result;
}

Vector3 EntityLivingBase::getLook(float ticktime)
{
	float projz;
	float projx;
	float projxz;
	float projy;

	if (ticktime == 1.0F)
	{
		projz = Math::Cos(-rotationYaw * Math::DEG2RAD - Math::PI);
		projx = Math::Sin(-rotationYaw * Math::DEG2RAD - Math::PI);
		projxz = -Math::Cos(-rotationPitch * Math::DEG2RAD);
		projy = Math::Sin(-rotationPitch * Math::DEG2RAD);
		return Vector3(projx * projxz, projy, projz * projxz);
	}
	else
	{
		projz = prevRotationPitch + (rotationPitch - prevRotationPitch) * ticktime;
		projx = prevRotationYaw + (rotationYaw - prevRotationYaw) * ticktime;
		projxz = Math::Cos(-projx * Math::DEG2RAD - Math::PI);
		projy = Math::Sin(-projx * Math::DEG2RAD - Math::PI);
		float var6 = -Math::Cos(-projz * Math::DEG2RAD);
		float var7 = Math::Sin(-projz * Math::DEG2RAD);
		return Vector3(projy * var6, var7, projxz * var6);
	}
	LordAssert(false);
}

float EntityLivingBase::getSwingProgress(float ticktime)
{
	if (isHoldGun)
	{
		float delta = gunFireProgress - prevGunFireProgress;
		if (delta < 0.f)
			++delta;
		return prevGunFireProgress + delta * ticktime;
	}
	else
	{
		float delta = swingProgress - prevSwingProgress;

		if (delta < 0.0F)
		{
			++delta;
		}

		return prevSwingProgress + delta * ticktime;
	}
	LordAssert(false);
}

RayTraceResult EntityLivingBase::rayTrace(float distance, float ticktime)
{
	Vector3 pos = getPosition(ticktime);
	Vector3 lookat = getLook(ticktime);
	Vector3 end = pos + lookat * distance;
	return world->rayTraceBlocks(pos, end);
}

bool EntityLivingBase::isClientWorld()
{
	return world->m_isClient; 
}

void EntityLivingBase::setBeenAttacked()
{
	beenAttacked = rand->nextDouble() >= getEntityAttribute(SharedMonsterAttributes::KNOCKBACK_RESISTANCE)->getAttributeValue();
}

void EntityLivingBase::setAbsorptionAmount(float amount)
{
	if (amount < 0.0F)
	{
		amount = 0.0F;
	}

	absorptionAmount = amount;
}

void EntityLivingBase::moveEntityInWater(float strafe, float forward, float dt)
{
	bool isEntityPlayer = isClass(ENTITY_CLASS_PLAYER);
	float tempY = position.y;
	moveFlying(strafe, forward, isAIEnabled() ? 0.04F : 0.02F);
	moveEntity(motion);

	motion *= 0.8f;
	motion.y -= 0.02f;
	
	if (isCollidedHorizontally && isOffsetPositionInLiquid(motion + Vector3(0.f, 0.6f - position.y + tempY, 0.f)))
	{
		motion.y = 0.3f;
	}
}

void EntityLivingBase::moveEntityInLava(float strafe, float forward, float dt)
{
	bool isEntityPlayer = isClass(ENTITY_CLASS_PLAYER);
	float tempY = position.y;
	moveFlying(strafe, forward, 0.02F);
	moveEntity(motion);
	motion *= 0.5f;
	motion.y -= 0.02f;
	
	if (isCollidedHorizontally && isOffsetPositionInLiquid(motion + Vector3(0.f, 0.6f - position.y + tempY, 0.f)))
	{
		motion.y = 0.3f;
	}
}

void EntityLivingBase::moveEntityWhenParachute(float strafe, float forward, float dt)
{
	float tempY = position.y;
	float speed = LogicSetting::Instance()->getParachuteMoveSpeed();
	Vector3 pos = position;
	moveFlying(strafe, forward, speed);
	moveEntity(motion);
	float downAccelerate = LogicSetting::Instance()->getParachuteDownSpeed();
	motion *= downAccelerate;
	motion.y -= 0.04f;
}

void EntityLivingBase::moveEntityFly(float strafe, float forward, float dt)
{
	moveFlying(strafe, forward, 0.02);
	moveEntity(motion);

	motion.y = 0.0f;
	motion.x = 0.0f;
	motion.z = 0.0f;
}

void EntityLivingBase::moveEntityCommon(float strafe, float forward, float dt)
{
	bool isEntityPlayer = isClass(ENTITY_CLASS_PLAYER);
	float speed_attenuation = 1.0F;
	float slipperiness = getSlipperiness(speed_attenuation);

	float f = 0.16277136F / (slipperiness * slipperiness * slipperiness);
	float speed = 0.0f;

	if (onGround)
	{
		speed = getAIMoveSpeed() * f + getSpeedAddition();
		speed = speed * speed_attenuation;
	}
	else
	{
		speed = jumpMovementFactor;
	}

	moveFlying(strafe, forward, speed);
	handleOnLadderBeforeMove(dt);
	moveEntity(motion);
	handleOnLadderAfterMove();
	handleEntityDrop(dt);

	motion.y *= 0.98f;
	motion.x *= slipperiness;
	motion.z *= slipperiness;
}

float EntityLivingBase::getSlipperiness(float& speed_attenuation)
{
	float slipperiness = 0.91F;

	if (onGround)
	{
		slipperiness = 0.54600006F;
		BlockPos pos = Vector3(position.x, boundingBox.vMin.y, position.z).getFloor();
		int blockID = world->getBlockId(pos.getNegY());

		if (blockID > 0)
		{
			slipperiness = BlockManager::sBlocks[blockID]->slipperiness() * 0.91F;
			speed_attenuation = BlockManager::sBlocks[blockID]->speed_attenuation();
		}
	}

	return slipperiness;
}

void EntityLivingBase::handleOnLadderBeforeMove(float dt)
{
	if (isOnLadder())
	{
		float maxMotion = 0.15F;

		if (motion.x < -maxMotion)
			motion.x = -maxMotion;
		if (motion.x > maxMotion)
			motion.x = maxMotion;
		if (motion.z < -maxMotion)
			motion.z = -maxMotion;
		if (motion.z > maxMotion)
			motion.z = maxMotion;

		fallDistance = 0.0F;

		if (motion.y < -0.15f)
			motion.y = -0.15f;

		bool sneaking = isSneaking() && isClass(ENTITY_CLASS_PLAYER);
		if (sneaking && motion.y < 0.0f)
		{
			motion.y = 0.0f;
		}
	}
}

void EntityLivingBase::handleOnLadderAfterMove()
{
	if (isCollidedHorizontally && isOnLadder())
	{
		motion.y = 0.2f;
	}
}

void EntityLivingBase::handleEntityDrop(float dt)
{
	// FIX [SYMPTOM-1]: the "chunk not loaded" fallback (fixed -0.1 vertical
	// velocity) must never fight a real jump. It exists to make entities drift
	// down slowly over void/unloaded terrain; when the entity is standing on
	// the ground (or actively jumping from it) vanilla gravity must apply,
	// otherwise jump() sets motion.y = 0.42 and this branch immediately
	// overwrites it with -0.1 -> the player can never clear a single block.
	if (world->m_isClient && !onGround &&
		(!world->blockExists(BlockPos((int)position.x, 0, (int)position.z)) ||
			!world->getChunkFromBlockCoords((int)position.x, (int)position.z)->m_isChunkLoaded))
	{
		if (position.y > 0.0f)
		{
			motion.y = -0.1f;
		}
		else
		{
			motion.y = 0.0f;
		}
	}
	else 
	{
		if (position.y > -128.f)
		{
			motion.y -= 0.08f;
		}
		else
		{
			motion.y = 0.0f;
		}
	}
}

}
