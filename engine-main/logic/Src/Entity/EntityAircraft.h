#pragma once
#include "Entity.h"

namespace BLOCKMAN
{
	class EntityPlayer;
	class EntityAircraft : public Entity
	{
		RTTI_DECLARE(EntityAircraft);
	public:
		EntityAircraft(World* pWorld, int id, const Vector3& pos, float yaw = 0.f);
		virtual ~EntityAircraft();

		void startPlaneTest();
		int getAircraftId() { return m_aircraftID; }
		void setAirLine(Vector3 startPos, Vector3 endPos, float speed);
		bool getOffPassenger(int playerEntityId);
		bool addPassenger(EntityPlayer* pPlayer);
		bool getOffAllPassengers();
		Vector3 getAirLineStartPosition() { return m_startPos; }
		Vector3 getAirLineEndPosition() { return m_endPos; }
		bool isAircraftFlying() { return m_bStartFlying; }
		const std::set<int>& getPassengers() { return m_passengerEntityIdList; }
		float getSpeed() { return m_fSpeed; }
		void setFlying(bool bFlying) { m_bStartFlying = bFlying; }
		bool canLeaveAircraft() { return m_nFlyTotalTick >= m_nCanJumpTick; }
		int getFlyingTick() { return m_nFlyTotalTick; }
		void setFlyingTick(int tick) { m_nFlyTotalTick = tick; }
		void RefreshPassengerPosition(Vector3 startPos);

		/** implement override functions from Entity */
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
		virtual bool canBePushed() { return false; }
		/** Checks whether target entity is alive. */
		virtual bool isEntityAlive() { return true; }
		/** Called to update the entity's position/logic. */
		virtual void onUpdate();
		/** Called frequently so the entity can update its state every tick as required. For example, zombies and skeletons
		* use this to react to sunlight and start to burn. */
		virtual void onAircraftUpdate();
		/** Handles updating while being ridden by an entity */
		virtual void updateRidden();
		/** Sets the position and rotation. Only difference from the other one is no bounding on the rotation. Args: posX, posY, posZ, yaw, pitch */
		virtual void setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment);
		/** returns a (normalized) vector of where this entity is looking */
		virtual Vector3 getLookVec() { return getLook(1.0F); }
		virtual Vector3 getLook(float ticktime);
		virtual void moveEntity(const Vector3& vel);
		virtual void whenRemoved();

		virtual float getEyeHeight() { return 1.2F; }
		Vector3 m_passengerEye;

	private:
		Vector3 m_airLine = Vector3(1.0f, 0.0f, 0.0f);   //direction
		Vector3 m_startPos = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 m_endPos = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 m_originPos;

		bool m_bStartFlying = false;
		float m_fSpeed = 10.0f;
		int m_nFlyTotalTick = 0;
		const int m_nCanJumpTick = 100;
		

		i32 playSoundTime = 0;
		/** the id for aircraft */
		int m_aircraftID = 0;
		/** The passenger of this aircraft. */
		std::set<int> m_passengerEntityIdList;

		void onArriveDestination();
		bool isOnClient();
		void lookAt(Vector2 dir);
	};
}

