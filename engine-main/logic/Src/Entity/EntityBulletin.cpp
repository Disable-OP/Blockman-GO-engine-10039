#include "EntityBulletin.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Setting/BulletinSetting.h"

namespace BLOCKMAN
{

	EntityBulletin::EntityBulletin(World* pWorld) : EntitySessionNpc(pWorld) {
		preventEntitySpawning = true;
	}

	EntityBulletin::EntityBulletin(World* pWorld, const Vector3& pos) : EntityBulletin(pWorld) 
	{ 
		setPosition(pos); 
	}

	EntityBulletin::~EntityBulletin()
	{
	}

	void EntityBulletin::onUpdate()
	{
		EntitySessionNpc::onUpdate();
	}

	void EntityBulletin::setBulletinId(i32 bulletinId)
	{
		m_bulletinId = bulletinId;
		const auto bulletin = BulletinSetting::getBulletinById(m_bulletinId);
		if (bulletin)
		{
			setNameLang(bulletin->name);
			setActorName(bulletin->actorName);
			setActorBody(bulletin->actorBody);
			setActorBodyId(bulletin->actorBodyId);
		}
	}
}