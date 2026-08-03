#include "EntityActorCannonClient.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "World/World.h"
#include "Chunk/Chunk.h"
#include "Common.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Setting/ActorSizeSetting.h"
#include "cWorld/Blockman.h"
#include "EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	EntityActorCannonClient::EntityActorCannonClient(World* pWorld, const Vector3& beginPos, const Vector3& fallOnPos)
		: EntityActorCannon(pWorld, beginPos, fallOnPos)
	{
	}

	EntityActorCannonClient::~EntityActorCannonClient()
	{
		ShowCannonBtnEvent::emit(false, 0);
	}

	void EntityActorCannonClient::onUpdate()
	{
		Entity::onUpdate();

		if (canFall())
		{
			moveEntity(motion);
		}

		Vector3 playerPos = Blockman::Instance()->m_pPlayer->position;
		playerPos.y -= 1.62f;
		if (playerPos.x > m_beginPos.x - 2.0f && playerPos.x < m_beginPos.x + 2.0f && playerPos.z > m_beginPos.z - 2.0f && playerPos.z < m_beginPos.z + 2.0f && playerPos.y > m_beginPos.y - 1.0f && playerPos.y < m_beginPos.y + 1.0f)
		{
			if (!m_isShowCannonBtn)
			{
				ShowCannonBtnEvent::emit(true, entityId);
				m_isShowCannonBtn = true;
			}
		}
		else
		{
			if (m_isShowCannonBtn)
			{
				ShowCannonBtnEvent::emit(false, 0);
				m_isShowCannonBtn = false;
			}
		}
	}
}