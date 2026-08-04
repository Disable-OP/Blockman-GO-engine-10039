#include "EntityVehicle.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Common.h"
#include "Script/Event/LogicScriptEvents.h"
#include "DamageSource.h"
#include "Enchantment.h"
#include "Entity/EntityPlayer.h"

namespace BLOCKMAN
{

EntityVehicle::EntityVehicle(World* pWorld, int id, const Vector3& pos, float yaw)
	: Entity(pWorld)
	, m_vehicleID(id)
{
	m_sync_oldPos = pos;
	prevRotationYaw = rotationYaw = yaw;
	setSize(2.f, 2.f, 1.3f);
	setPosition(pos);
	serverPos.x = int(Math::Floor(pos.x * 32.f));
	serverPos.y = int(Math::Floor(pos.y * 32.f));
	serverPos.z = int(Math::Floor(pos.z * 32.f));
	m_passengerEye = Vector3(0.f, 2.f, 0.f);
	m_showPassenger = false; // for debug.
	m_bisLocked = false;
	m_hasDriver = false;
	m_maxPassengers = size_t(4);
	m_prevState = VehicleActionState::IDLE;
	m_curState = VehicleActionState::IDLE;

	// configure all.
	m_maxVelocity = 20.f / 20.f;
	m_addVelocity = 2.f / 20.f;
	m_resistance = 1.f / 20.f;
	m_angleVelocity = 30.f / 20.f;
	m_minTurnOff = 4.f / 20.f;
	m_currentVelocity = 0.f;

	auto setting = getSetting();
	if (setting)
	{
		m_maxPassengers = size_t(setting->passengers);
		m_maxVelocity = setting->maxSpeed;
		m_addVelocity = setting->traction / setting->weight;
		m_addVelocity = m_addVelocity > m_resistance ? m_addVelocity : m_resistance + 0.01f; // incase of can not move
		m_angleVelocity = setting->turnSpeed;
	}

	m_driverEntityId = 0;
}

EntityVehicle::~EntityVehicle()
{
	m_passengerEntityIdList.clear();
	m_driverEntityId = 0;
}

void EntityVehicle::updateFallState(float y, bool onGroundIn)
{
	handleWaterMovement();

	if (onGroundIn && fallDistance > 0.0f)
	{
		BlockPos pos = (position - Vector3(0.f, 0.2f + yOffset, 0.f)).getFloor();
		int blockID = world->getBlockId(pos);

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

void EntityVehicle::stopVehicle()
{
	m_moveStrafing = 0.0f;
	m_moveForward = 0.0f;
	m_currentVelocity = 0.0f;
	motion.x = 0.0f;
	motion.z = 0.0f;
	setActionState(VehicleActionState::IDLE);
}

void EntityVehicle::checkActionState()
{
	VehicleActionState state = VehicleActionState::IDLE;
	if (m_moveForward > 0.0f || m_currentVelocity > 0.0f)
	{
		state = VehicleActionState::RUN;
	}
	else if(m_moveForward < 0.0f || m_currentVelocity < 0.0f)
	{
		state = VehicleActionState::BACK;
	}

	if (state != m_curState)
	{
		m_curState = state;
	}

	//BLOCKMAN::GameCommon::Instance()->getCommonPacketSender()->sendVehicleActionState(entityId, (int)state);
}

void EntityVehicle::fall(float distance)
{
	Entity::fall(distance);
	int dis = int(Math::Ceil(distance - 3.0F));

	if (dis >= -1)
	{
		if (dis > 3)
			playSoundByType(ST_FallBig);
		else
			playSoundByType(ST_FallSmall);
	}

}

void EntityVehicle::kill()
{
	// attackEntityFrom(DamageSource::outOfWorld, 4.0F);
}


void EntityVehicle::onEntityUpdate()
{
	prevPos = position;
	prevRotationYaw = rotationYaw;

	if (isEntityAlive() && isEntityInsideOpaqueBlock())
	{
		// attackEntityFrom(DamageSource::inWall, 1.0F);
	}

	if (isEntityAlive() && isInsideOfMaterial(BM_Material::BM_MAT_water))
	{
		if (!world->m_isClient)
		{
			mountEntity(NULL);
		}
	}
	else
	{
		setAir(300);
	}

	if (position.y < -64.f && !isClientWorld())
	{
		setDead();
	}

	m_prevCameraPitch = m_cameraPitch;

	m_prevMovedDistance = m_movedDistance;
	m_prevRenderYawOffset = m_renderYawOffset;
	prevRotationYaw = rotationYaw;
	prevRotationPitch = rotationPitch;
}

void EntityVehicle::readEntityFromNBT(NBTTagCompound* pNBT)
{
}

void EntityVehicle::writeEntityToNBT(NBTTagCompound* pNBT)
{
}


bool EntityVehicle::attackEntityFrom(DamageSource* pSource, float amount)
{
	return false;
}

void EntityVehicle::onUpdate()
{
	Entity::onUpdate();

	onVehicleUpdate();
	checkActionState();
}

void EntityVehicle::moveEntity(const Vector3& vel)
{
	Vector3 prePos = position;
	
	yPositionDecrement *= 0.4F;
	Vector3 pos = position;
	float x = vel.x;
	float y = vel.y;
	float z = vel.z;
	
	float tx = x;
	float ty = y;
	float tz = z;
	Box bb = boundingBox;
	
	AABBList& aabbs = world->getCollidingBoundingBoxes(this, boundingBox.addCoord(x, y, z));
	if (y != 0.0f) 
	{
		for (int i = 0; i < int(aabbs.size()); ++i)
		{
			y = aabbs[i].getYOffset(boundingBox, y);
		}
		if (y != 0.0f)
		{
			boundingBox.move(0.0f, y, 0.0f);
		}
	}
	
	bool canTouchGround = onGround || ty != y && ty < 0.0f;

	// can jump
	float papo = 0.f;
	for (int i = 0; i < int(aabbs.size()); ++i)
	{
		if (aabbs[i].vMax.y - boundingBox.vMin.y > papo)
			papo = aabbs[i].vMax.y - boundingBox.vMin.y;
	}
	if (papo <= 1.1f)
	{
		boundingBox.move(0.0f, papo, 0.0f);
	}

	int i;
	if (x != 0.0f) 
	{
		for (i = 0; i < int(aabbs.size()); ++i)
		{
			x = aabbs[i].getXOffset(boundingBox, x);
		}
		if (x != 0.0f) {
			boundingBox.move(x, 0.0f, 0.0f);
		}
	}

	if (z != 0.0f) 
	{
		for (i = 0; i < int(aabbs.size()); ++i)
		{
			z = aabbs[i].getZOffset(boundingBox, z);
		}

		if (z != 0.0f) {
			boundingBox.move(0.0f, 0.0f, z);
		}
	}

	position.x = (boundingBox.vMin.x + boundingBox.vMax.x) / 2.0f;
	position.y = boundingBox.vMin.y + yOffset - yPositionDecrement;
	position.z = (boundingBox.vMin.z + boundingBox.vMax.z) / 2.0f;
	isCollidedHorizontally = (Math::Abs(tx - x) > 0.001f) || Math::Abs(tz - z) > 0.001f;
	isCollidedVertically = (Math::Abs(ty - y) > 0.001f);
	onGround = isCollidedVertically && ty < 0.0f;
	isCollided = isCollidedHorizontally || isCollidedVertically;
	updateFallState(y, onGround);

	if (tx != x)
		motion.x = 0.0f;

	if (ty != y)
		motion.y = 0.0f;

	if (tz != z)
		motion.z = 0.0f;

	float dx = position.x - pos.x;
	float dy = position.y - pos.y;
	float dz = position.z - pos.z;

	if (canTriggerWalking() && ridingEntity == NULL)
	{
		BlockPos pos = (position + Vector3(0.f, -0.2f - yOffset, 0.f)).getFloor();
		int blockID = world->getBlockId(pos);

		if (blockID == 0)
		{
			pos = pos.getNegY();
			int blockID1 = world->getBlockId(pos.getNegY());

			if (blockID1 == BLOCK_ID_FENCE ||
				blockID1 == BLOCK_ID_COBBLE_STONE_WALL ||
				blockID1 == BLOCK_ID_FENCE_GATE)
			{
				blockID = blockID1;
			}
		}

		if (blockID != BLOCK_ID_LADDER)
			dy = 0.0f;

		distanceWalkedModified = distanceWalkedModified + Math::Sqrt(dx * dx + dz * dz) * 0.6f;
		distanceWalkedOnStepModified = distanceWalkedOnStepModified + Math::Sqrt(dx * dx + dy * dy + dz * dz) * 0.6f;

		if (distanceWalkedOnStepModified > nextStepDistance && blockID > 0)
		{
			nextStepDistance = (int)distanceWalkedOnStepModified + 1;

			if (isInWater())
			{
				float var42 = Math::Sqrt(motion.x * motion.x * 0.2f + motion.y * motion.y + motion.z * motion.z * 0.2f) * 0.35F;

				if (var42 > 1.0F)
					var42 = 1.0F;

				//playSound("liquid.swim", var42, 1.0F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
				if (world->canPlaySpecialSound(2))
				{
					playSoundByType(ST_Swim);
				}
			}

			playStepSound(pos, blockID);
			BlockManager::sBlocks[blockID]->onEntityWalking(world, pos, this);
		}
	}
}

void EntityVehicle::onVehicleUpdate()
{
	if (Math::Abs(motion.x) < 0.005f)
		motion.x = 0.0f;
	if (Math::Abs(motion.y) < 0.005f)
		motion.y = 0.0f;
	if (Math::Abs(motion.z) < 0.005f)
		motion.z = 0.0f;

	if (isMovementBlocked())
	{
		m_isJumping = false;
		m_moveStrafing = 0.0F;
		m_moveForward = 0.0F;
	}
	else
	{
		updateEntityActionState();
	}
	
	moveEntityWithHeading(m_moveStrafing, m_moveForward);
	motion.x = 0.0f;
	motion.z = 0.0f;

	collideWithNearbyEntities();

	syncPositionToPassenger();
}

void EntityVehicle::updateRidden()
{
	Entity::updateRidden();
	m_prevOnGroundSpeedFactor = m_onGroundSpeedFactor;
	m_onGroundSpeedFactor = 0.0F;
	fallDistance = 0.0F;
}

void EntityVehicle::jump()
{
	motion.y = 0.42f;
	isAirBorne = true;
}

void EntityVehicle::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
{
	prevPos = position;
	setPositionAndRotation(pos, yaw, pitch);
}

bool EntityVehicle::isClientWorld()
{
	return world->m_isClient;
}

const CarSetting * EntityVehicle::getSetting()
{
	return CarSetting::getCarSetting(m_vehicleID);
}

bool EntityVehicle::isOnLadder()
{
	BlockPos pos = Vector3(position.x, boundingBox.vMin.y, position.z).getFloor();
	int blockID = world->getBlockId(pos);
	return blockID == BLOCK_ID_LADDER || blockID == BLOCK_ID_VINE;
}

void EntityVehicle::collideWithNearbyEntities()
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

int EntityVehicle::getFreeSeatNum()
{
	return m_maxPassengers - m_passengerEntityIdList.size();
}

bool EntityVehicle::carryPassenger(int entityPlayerId, bool bIsDriver)
{
	if (bIsDriver)
	{
		m_driverEntityId = entityPlayerId;
		m_hasDriver = true;
		return true;
	}

	if (m_passengerEntityIdList.size() < m_maxPassengers)
	{
		m_passengerEntityIdList.push_back(entityPlayerId);
		return true;
	}

	return false;
}

bool EntityVehicle::getOffPassenger(int entityPlayerId)
{
	if (entityPlayerId == m_driverEntityId)
	{
		m_driverEntityId = 0;
		m_hasDriver = false;
		m_bisLocked = false;
		stopVehicle();
		return true;
	}

	for (list<int>::iterator iter = m_passengerEntityIdList.begin(); iter != m_passengerEntityIdList.end(); iter++)
	{
		if (*iter == entityPlayerId)
		{
			m_passengerEntityIdList.erase(iter);
			return true;
		}
	}
	
	return false;
}

void EntityVehicle::moveEntityWithHeading(float strafe, float forward)
{
	float dt = world->getFrameDeltaTime();
	float framePerSecond = 20.0f;
	m_currentVelocity += forward * m_addVelocity * dt * framePerSecond;
	if (m_currentVelocity > 0.f)
	{
		m_currentVelocity -= m_resistance * dt * framePerSecond;
		if (m_currentVelocity < 0.f)
			m_currentVelocity = 0.f;
	}
	else if (m_currentVelocity < 0.f)
	{
		m_currentVelocity += m_resistance * dt * framePerSecond;
		if (m_currentVelocity > 0.f)
			m_currentVelocity = 0.f;
	}

	if (m_currentVelocity > m_maxVelocity)
		m_currentVelocity = m_maxVelocity;
	if (m_currentVelocity < -m_maxVelocity)
		m_currentVelocity = -m_maxVelocity;

	if(m_currentVelocity > m_minTurnOff)
		rotationYaw -= strafe * m_angleVelocity * dt * framePerSecond;
	if (m_currentVelocity < -m_minTurnOff)
		rotationYaw += strafe * m_angleVelocity * dt * framePerSecond;

	float f1 = Math::Sin(rotationYaw * Math::PI / 180.0F);
	float f2 = Math::Cos(rotationYaw * Math::PI / 180.0F);
	motion.x = -f1 * m_currentVelocity * dt * framePerSecond;
	motion.z = f2 * m_currentVelocity * dt * framePerSecond;
	motion.y -= 0.08f;

	if (m_hasDriver || !onGround)
	{
		moveEntity(motion);
	}

	float deltax = position.x - prevPos.x;
	float deltaz = position.z - prevPos.z;
	m_moving = (deltax * deltax + deltaz * deltaz) > 0.004f;
	float delta = Math::Sqrt(deltax * deltax + deltaz * deltaz) * 4.0F;

	if (delta > 1.0F)
	{
		delta = 1.0F;
	}
}

Vector3 EntityVehicle::getLook(float ticktime)
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

bool EntityVehicle::lockVehicle(EntityPlayer* pPlayer, bool bState)
{
	if (!pPlayer)
		return false;

	if (!isDriver(pPlayer->entityId))
		return false;

	if (bState == m_bisLocked)
		return false;

	m_bisLocked = bState;
	return true;
}

bool EntityVehicle::canDrivedBy(EntityPlayer * pPlayer)
{
	if (CarSetting::m_sbIsCarFree)
		return true;

	const CarSetting* pSetting = getSetting();
	if (!pSetting)
		return true;

	if (pSetting->price <= 0)
		return true;

	if (pPlayer->hasOwnVehicle(m_vehicleID))
		return true;

	return false;
}

bool EntityVehicle::shootTargetByGun(EntityPlayer* pTarget, const C2SPACKET_DETAIL::Vector3& hitpos, const GunSetting* pGunSetting)
{
	if (m_driverEntityId != 0 && m_hasDriver)
	{
		EntityPlayer* pDriver = world->getPlayerEntity(m_driverEntityId);
		if(pDriver != NULL && !pDriver->capabilities.disableDamage)
			damageCalc(pTarget, pDriver, pGunSetting, hitpos);
	}

	std::list<int> tempList = m_passengerEntityIdList;
	for (int passengerId : tempList)
	{
		EntityPlayer* passenger = world->getPlayerEntity(passengerId);
		if (passenger != NULL && !passenger->capabilities.disableDamage)
			damageCalc(pTarget, passenger, pGunSetting, hitpos);
	}

	return true;
}

bool EntityVehicle::damageCalc(EntityPlayer * pTarget, EntityPlayer * hitPlayer, const GunSetting* pGunSetting, Vector3 hitpos) {
	const CarSetting* pSetting = CarSetting::getCarSetting(this->m_vehicleID);
	if (pSetting) {
		SCRIPT_EVENT::EntityHitEvent::invoke(pGunSetting->adaptedBulletId, hitpos, hitPlayer->entityId);
		float amount = pGunSetting->damage;
		amount *= pSetting->passengersDamage;

		DamageSource* pSource = NULL;
		pSource = DamageSource::causePlayerDamage(pTarget);
		pSource->knockBackDis = pGunSetting->knockbackDistance;

		if (hitPlayer->attackEntityFrom(pSource, amount))
		{
			EnchantmentThorns::damageArmor(pTarget, hitPlayer, rand);
		}
		LordSafeDelete(pSource);
	}
	return true;
}

void EntityVehicle::whenRemoved()
{
	if (m_driverEntityId != 0 && m_hasDriver)
	{
		EntityPlayer* pDriver = world->getPlayerEntity(m_driverEntityId);
		if (pDriver != NULL)
		{
			pDriver->takeOffVehicle();
			m_driverEntityId = 0;
			m_hasDriver = false;
		}
	}
		
	std::list<int> temp = m_passengerEntityIdList;
	for (list<int>::iterator iter = temp.begin(); iter != temp.end(); iter++)
	{
		auto player = world->getPlayerEntity(*iter);
		if(player != NULL)
			player->takeOffVehicle();
	}

	m_passengerEntityIdList.clear();
}

void EntityVehicle::syncPositionToPassenger()
{
	// copy all the pos to passenger.
	if (m_hasDriver && m_driverEntityId != 0)
	{
		EntityPlayer* pDriver = world->getPlayerEntity(m_driverEntityId);
		if (pDriver != NULL)
		{
			pDriver->prevPos = prevPos + m_passengerEye;
			pDriver->setPosition(position + m_passengerEye);
			pDriver->lastTickPos = lastTickPos + m_passengerEye;
		}
	}

	for (int passengerId : m_passengerEntityIdList)
	{
		EntityPlayer* passenger = world->getPlayerEntity(passengerId);
		if (passenger != NULL)
		{
			passenger->prevPos = prevPos + m_passengerEye;
			passenger->setPosition(position + m_passengerEye);
			passenger->lastTickPos = lastTickPos + m_passengerEye;
		}
	}
}

}