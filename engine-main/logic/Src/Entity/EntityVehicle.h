/********************************************************************
filename: 	EntityVehicle.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2018-01-27
*********************************************************************/
#ifndef __ENTITY_VEHICLE_HEADER__
#define __ENTITY_VEHICLE_HEADER__

#include "Entity.h"
#include "Setting/CarSetting.h"
#include "Network/NetworkDefine.h"

namespace BLOCKMAN
{

class EntityPlayer;

enum class VehicleActionState
{
	IDLE = 0,
	RUN,
	BACK,
};

class EntityVehicle: public Entity
{
	RTTI_DECLARE(EntityVehicle);

public:
	/** the id for vehicle */
	int m_vehicleID = 0;

	/** The passenger of this arrow. */
	int m_driverEntityId = 0;
	std::list<int> m_passengerEntityIdList;

	float m_maxVelocity = 20.f / 20.f;
	float m_addVelocity = 2.f / 20.f;
	float m_resistance = 1.f / 20.f;
	float m_angleVelocity = 30.f / 20.f;
	float m_minTurnOff = 4.f / 20.f;
	float m_currentVelocity = 0.f;

	int m_entityAge = 0;
	float m_prevOnGroundSpeedFactor = 0.f;
	float m_onGroundSpeedFactor = 0.f;
	float m_movedDistance = 0.f;
	float m_prevMovedDistance = 0.f;
	float m_unused180 = 0.f;
	bool m_isJumping = false;
	int m_newPosRotationIncrements = 0;
	Vector3 m_newPos;
	float m_newRotationYaw = 0.f;
	float m_newRotationPitch = 0.f;
	float m_landMovementFactor = 0.f;
	int m_jumpTicks = 0;
	float m_absorptionAmount = 0.f;
	float m_prevCameraPitch = 0.f;
	float m_cameraPitch = 0.f;
	float m_randomUnused2 = 0.f;
	float m_randomUnused1 = 0.f;
	float m_renderYawOffset = 0.f;
	float m_prevRenderYawOffset = 0.f;
	float m_fallMovementFactor = 0.02f; // = 0.02F;
	float m_moveStrafing = 0.f;
	float m_moveForward = 0.f;
	bool m_moving = false;
	Vector3 m_passengerEye;
	bool m_showPassenger = false;

	bool m_isClient = false;
	Vector3 m_sync_oldPos;
	float m_sync_oldMinY = 0.f;
	float m_sync_oldYaw = 0.f;
	int m_moveTick = 0;
	bool m_sync_wasOnGround = false;
	bool m_sync = false;

	bool m_hasMoved = false;
	bool m_bisLocked = false;
	bool m_hasDriver = false;
	size_t  m_maxPassengers = 0;
	VehicleActionState m_curState = VehicleActionState::IDLE;
	VehicleActionState m_prevState = VehicleActionState::IDLE;
public:
	EntityVehicle(World* pWorld, int id, const Vector3& pos, float yaw = 0.f);
	~EntityVehicle();

	/** implement override functions from Entity */

	/** implement override functions */
	virtual void entityInit() {}
	virtual void updateFallState(float y, bool onGroundIn);
	virtual void fall(float distance);
	virtual void kill();

	virtual bool isMovementBlocked() { return false; }
	/** Causes this entity to do an upwards motion (jumping). */
	virtual void jump();
	/** Gets called every tick from main Entity class */
	virtual void onEntityUpdate();
	/** (abstract) Protected helper method to read subclass entity data from NBT. */
	virtual void readEntityFromNBT(NBTTagCompound* pNBT);
	/** (abstract) Protected helper method to write subclass entity data to NBT. */
	virtual void writeEntityToNBT(NBTTagCompound* pNBT);
	/** Called when the entity is attacked. */
	virtual bool attackEntityFrom(DamageSource* pSource, float amount);
	/** Returns true if other Entities should be prevented from moving through this Entity. */
	virtual bool canBeCollidedWith() { return true; }
	/** Returns true if this entity should push and be pushed by other entities when colliding. */
	virtual bool canBePushed() { return true; }
	/** Checks whether target entity is alive. */
	virtual bool isEntityAlive() { return true; }
	/** Called to update the entity's position/logic. */
	virtual void onUpdate();
	/** Called frequently so the entity can update its state every tick as required. For example, zombies and skeletons
	* use this to react to sunlight and start to burn. */
	virtual void onVehicleUpdate(); 
	/** Handles updating while being ridden by an entity */
	virtual void updateRidden();
	/** Sets the position and rotation. Only difference from the other one is no bounding on the rotation. Args: posX, posY, posZ, yaw, pitch */
	virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
	/** returns a (normalized) vector of where this entity is looking */
	virtual Vector3 getLookVec() { return getLook(1.0F); }
	virtual Vector3 getLook(float ticktime);
	virtual void moveEntity(const Vector3& vel);
	
	virtual float getEyeHeight() { return 1.2F; }

	void setMoveForward(float speed) { m_moveForward = speed; }
	void setMoveStrafe(float strafe) { m_moveStrafing = strafe; }
	void collideWithNearbyEntities();
	void collideWithEntity(Entity* pEntity) { pEntity->applyEntityCollision(this); }
	void setJumping(bool isjump) { m_isJumping = isjump; }
	int getFreeSeatNum();
	bool carryPassenger(int entityPlayerId, bool bIsDriver);
	bool getOffPassenger(int entityPlayerId);
	bool isDriver(int entityPlayerId) { return m_driverEntityId > 0 && m_driverEntityId == entityPlayerId; }
	void stopVehicle();
	void setActionState(VehicleActionState state) { m_prevState = m_curState; m_curState = state; }
	void checkActionState();

	int getAge() { return m_entityAge; }
	bool isOnLadder();

	/** Move the entity to the coordinates informed, but keep yaw/pitch values. */
	void setPositionAndUpdate(const Vector3& pos) { setLocationAndAngles(pos, rotationYaw, rotationPitch); }
	/** set position and yaw*/
	void setPositionAndYaw(const Vector3& pos, float yaw) { setLocationAndAngles(pos, yaw, rotationPitch); }
	/** Moves the entity based on the specified heading.  Args: strafe, forward */
	virtual void moveEntityWithHeading(float strafe, float forward);
	/** the movespeed used for the new AI system */
	virtual bool isClientWorld();
	virtual void updateEntityActionState() { ++m_entityAge; }

	const CarSetting* getSetting();
	bool lockVehicle(EntityPlayer* pPlayer, bool bState);
	bool canDrivedBy(EntityPlayer* pPlayer);
	bool hasPlayer() { return m_hasDriver || m_passengerEntityIdList.size() > 0; }
	bool shootTargetByGun(EntityPlayer * pTarget, const C2SPACKET_DETAIL::Vector3& hitpos, const GunSetting* pGunSetting);
	bool damageCalc(EntityPlayer * pTarget, EntityPlayer * hitPlayer, const GunSetting* pGunSetting, Vector3 hitpos);
	virtual void whenRemoved();
	void setCurrentVelocity(float fVelocity) { m_currentVelocity = fVelocity; }

private:
	void syncPositionToPassenger();
};

}

#endif