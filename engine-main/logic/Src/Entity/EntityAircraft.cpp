#include "EntityAircraft.h"
#include "Entity/EntityPlayer.h"
#include "World/World.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Audio/SoundSystem.h"

namespace BLOCKMAN
{
	EntityAircraft::EntityAircraft(World * pWorld, int id, const Vector3 & pos, float yaw)
		: Entity(pWorld)
		, m_aircraftID(id)
		, m_bStartFlying(false)
	{
		m_originPos = pos;
		setPosition(pos);
		rotationYaw = yaw;
		rotationPitch = 0.0f;
		m_fSpeed = 10.0f;
		m_passengerEye = Vector3(0.f, 5.f, 0.f);
	}

	EntityAircraft::~EntityAircraft()
	{
	}

	void EntityAircraft::startPlaneTest()
	{
		auto startPos = Vector3(position.x - 20.0f, position.y + 100.0f, position.z - 20.0f);
		auto endPos = Vector3(position.x + 20.0f, position.y + 100.0f, position.z + 20.0f);
		setAirLine(startPos, endPos, 10.0f);
		position = startPos;
	}

	void EntityAircraft::RefreshPassengerPosition(Vector3 startPos) {
		for (auto it = m_passengerEntityIdList.begin(); it != m_passengerEntityIdList.end(); it++)
		{
			EntityPlayer* passenger = world->getPlayerEntity(*it);
			if (passenger != NULL)
			{
				passenger->position = position + m_passengerEye;
				passenger->rotationYaw = rotationYaw;
				passenger->rotationPitch = rotationPitch;
			}
		}
	}

	void EntityAircraft::setAirLine(Vector3 startPos, Vector3 endPos, float speed)
	{
		m_startPos = startPos;
		m_endPos = endPos;
		m_airLine = m_endPos - m_startPos;
		m_airLine.normalize();
		m_bStartFlying = true;
		m_nFlyTotalTick = 0;
		m_passengerEye = -10 * Vector3(m_airLine.x, 0, m_airLine.z) + Vector3(0.0f, 5.0f, 0.0f);
		lookAt(Vector2(m_airLine.x, m_airLine.z));
		/*
		LordLogInfo("EntityAircraft::setAirLine (%f, %f, %f) -> (%f, %f, %f), speed %f, eye (%f, %f, %f)", m_startPos.x, m_startPos.y, m_startPos.z, 
			m_endPos.x, m_endPos.y, m_endPos.z, speed, m_passengerEye.x, m_passengerEye.y, m_passengerEye.z);
		*/
	}

	bool EntityAircraft::getOffPassenger(int playerEntityId)
	{
		auto iter = m_passengerEntityIdList.find(playerEntityId);
		if (iter != m_passengerEntityIdList.end())
		{
			m_passengerEntityIdList.erase(iter);
			return true;
		}

		return false;
	}

	bool EntityAircraft::addPassenger(EntityPlayer* pPlayer)
	{
		if (pPlayer == NULL)
			return false;

		int playerEntityId = pPlayer->entityId;
		auto iter = m_passengerEntityIdList.find(playerEntityId);
		if (iter != m_passengerEntityIdList.end())
			return false;

		m_passengerEntityIdList.insert(playerEntityId);
		pPlayer->position = position + m_passengerEye;
		pPlayer->rotationYaw = rotationYaw;
		pPlayer->rotationPitch = rotationPitch;
		/*
		LordLogInfo("EntityAircraft::addPassenger Id %d, pos (%f, %f, %f) yaw %f pitch %f", pPlayer->entityId,  pPlayer->position.x, pPlayer->position.y, pPlayer->position.z, 
			pPlayer->rotationYaw, pPlayer->rotationPitch);
		*/
		return true;
	}

	bool EntityAircraft::getOffAllPassengers()
	{
		std::set<int> temp = m_passengerEntityIdList;
		for (auto it = temp.begin(); it != temp.end(); it++)
		{
			EntityPlayer* pPassenger = world->getPlayerEntity(*it);
			if (pPassenger != NULL)
			{
				pPassenger->takeOffAircraft();
			}
		}

		m_passengerEntityIdList.clear();
		return true;
	}

	void EntityAircraft::updateFallState(float y, bool onGroundIn)
	{
	}
	void EntityAircraft::fall(float distance)
	{
	}
	void EntityAircraft::kill()
	{
	}
	void EntityAircraft::jump()
	{
	}
	void EntityAircraft::onEntityUpdate()
	{
	}
	void EntityAircraft::readEntityFromNBT(NBTTagCompound * pNBT)
	{
	}
	void EntityAircraft::writeEntityToNBT(NBTTagCompound * pNBT)
	{
	}
	bool EntityAircraft::attackEntityFrom(DamageSource * pSource, float amount)
	{
		return false;
	}
	void EntityAircraft::onUpdate()
	{
		Entity::onUpdate();
		onAircraftUpdate();
	}

	void EntityAircraft::onAircraftUpdate()
	{
		if (!m_bStartFlying)
			return;

		m_nFlyTotalTick++;
		prevRotationYaw = rotationYaw;
		prevPos = position;
		bool bArrived = false;
		float dt = world->getFrameDeltaTime();
		//LordLogInfo("onAircraftUpdate frame time is %f", dt);
		float delta = dt * m_fSpeed;
		auto pos = position + delta * m_airLine;
		float distance = (m_endPos - position).lenSqr();
		float moveDelta = (delta * m_airLine).lenSqr();
		if (moveDelta >= distance)
		{
			pos = m_endPos;
			bArrived = true;
		}

		setPosition(pos);
		if (world->m_isClient && playSoundTime == 0)
		{
			playSoundTime = 1;
			playSoundByType(SoundType::ST_AircraftSound);
		}

		if (m_nFlyTotalTick % 10 == 0 && !isOnClient())
		{
			SCRIPT_EVENT::AircraftMoveEvent::invoke(entityId, pos);
		}
	
		for (auto it = m_passengerEntityIdList.begin(); it != m_passengerEntityIdList.end(); it++)
		{
			EntityPlayer* pPlayer = world->getPlayerEntity(*it);
			if (pPlayer != NULL)
			{
				pPlayer->prevPos = prevPos + m_passengerEye;
				pPlayer->setPosition(position + m_passengerEye);
				pPlayer->lastTickPos = lastTickPos + m_passengerEye;
			}
		}

		if (bArrived && !isOnClient())
		{
			onArriveDestination();
		}
	}   

	void EntityAircraft::updateRidden()
	{
	}

	void EntityAircraft::setPositionAndRotation2(const Vector3 & pos, float yaw, float pitch, int increment)
	{
		prevPos = position;
		setPosition(pos);
		setRotation(yaw, pitch);
	}

	Vector3 EntityAircraft::getLook(float ticktime)
	{
		return Vector3();
	}
	void EntityAircraft::moveEntity(const Vector3 & vel)
	{
	}

	void EntityAircraft::whenRemoved()
	{
		getOffAllPassengers();
	}

	void EntityAircraft::onArriveDestination()
	{
		getOffAllPassengers();
		m_bStartFlying = false;
		setDead();
		if (world->m_isClient)
		{
			SoundSystem::Instance()->stopBackgroundMusic();
		}
	}

	bool EntityAircraft::isOnClient()
	{
		return world->m_isClient;
	}

	void EntityAircraft::lookAt(Vector2 dir)
	{
		auto yaw = Math::ASin(-dir.x);
		yaw = yaw * Math::RAD2DEG;
		if (dir.y < 0)
		{
			yaw = 180.0f - yaw;
		}

		rotationYaw = yaw;
		rotationPitch = 30.0f;
	}
}



