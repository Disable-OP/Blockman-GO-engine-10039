#include "EntityActorCannon.h"
//#include "Block/Block.h"
//#include "Block/BlockManager.h"
#include "World/World.h"
//#include "Chunk/Chunk.h"
//#include "Common.h"
//#include "Script/Event/LogicScriptEvents.h"
#include "Setting/ActorSizeSetting.h"

namespace BLOCKMAN
{
	EntityActorCannon::EntityActorCannon(World* pWorld, const Vector3& beginPos, const Vector3& fallOnPos)
		: Entity(pWorld)
		, m_beginPos(beginPos)
		, m_fallOnPos(fallOnPos)
	{
		preventEntitySpawning = true;
		setPosition(beginPos);

		m_launchPoint = beginPos;
		float deltaX = fallOnPos.x - beginPos.x;
		float deltaZ = fallOnPos.z - beginPos.z;
		float lensqr = Math::Sqrt(deltaX * deltaX + deltaZ * deltaZ);

		if (lensqr < 0.00001f)
			lensqr = 0.00001f;

		m_launchPoint.x += deltaX / lensqr * 2.0f;
		m_launchPoint.y += 1.0f;
		m_launchPoint.z += deltaZ / lensqr * 2.0f;
		m_remainingFlyTicks = 0;

		// calculate launching motion
		float maxHeight = m_launchPoint.y > fallOnPos.y ? m_launchPoint.y + 5.0f : fallOnPos.y + 5.0f;
		float vx0, vy0, vz0;
		float height;
		float vSum = 0.0f;
		float vTemp = 0.0f;
		height = maxHeight - m_launchPoint.y;
// 		while (vSum < height)
// 		{
// 			vTemp = vTemp * 0.98f;
// 			vSum -= vTemp;
// 			vTemp = (vTemp - 0.08f) * 0.98f;
// 			m_remainingFlyTicks++;
// 		}
// 		vy0 = -vTemp;
// 		height = vSum - height + maxHeight - fallOnPos.y;
// 		vSum = 0.0f;
// 		vTemp = 0.0f;
		while (vSum < height)
		{
			vTemp = vTemp / 0.98f + 0.08f;
			vSum += vTemp;
			vTemp = vTemp / 0.98f;
			m_remainingFlyTicks++;
		}
		vy0 = vTemp;
		vSum = 0.0f;
		int tempTicks = 0;
		while (vTemp > 0.0f)
		{
			vTemp = vTemp * 0.98f;
			vSum += vTemp;
			vTemp = (vTemp - 0.08f) * 0.98f;
			tempTicks++;
		}

		height = vSum - height + maxHeight - fallOnPos.y;
		vSum = 0.0f;
		vTemp = 0.0f;
		while (vSum < height)
		{
			vTemp *= 0.98f;
			vSum -= vTemp;
			vTemp = (vTemp - 0.08f) * 0.98f;
			m_remainingFlyTicks++;
		}

		float maxDistanceX = fallOnPos.x - m_launchPoint.x;
		vx0 = maxDistanceX * (1.0f - 0.98f * 0.91f) / 0.98f / (1.0f - Math::Pow(0.98f * 0.91f, (float)m_remainingFlyTicks));

		float maxDistanceZ = fallOnPos.z - m_launchPoint.z;
		vz0 = maxDistanceZ * (1.0f - 0.98f * 0.91f) / 0.98f / (1.0f - Math::Pow(0.98f * 0.91f, (float)m_remainingFlyTicks));

		m_motion = Vector3(vx0, vy0, vz0);
	}

	EntityActorCannon::~EntityActorCannon()
	{
	}

	void EntityActorCannon::onUpdate()
	{
		Entity::onUpdate();

		if (canFall())
		{
			moveEntity(motion);
		}
	}

	void EntityActorCannon::setActorName(const String& actorName)
	{
		m_actorName = actorName;
		ActorSize* pSize = ActorSizeSetting::getActorSize(m_actorName);
		setSize(pSize->width, pSize->length, pSize->height);
		setPosition(position);
	}
}