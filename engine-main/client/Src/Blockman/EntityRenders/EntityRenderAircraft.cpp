#include "EntityRenderAircraft.h"
#include "Entity/EntityAircraft.h"
#include "bm_clientDef.h"
#include "EntityRenderManager.h"

#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"

#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorTemplateData.h"

namespace BLOCKMAN
{
	EntityRenderAircraft::EntityRenderAircraft()
	{
	}
	EntityRenderAircraft::~EntityRenderAircraft()
	{
	}
	void EntityRenderAircraft::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto pAircraft = dynamic_cast<EntityAircraft*>(pEntity);
		if (!pAircraft)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pAircraft);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pAircraft->prevPos + (pAircraft->position - pAircraft->prevPos) * rdt;
		position.y -= pAircraft->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = pAircraft->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
	}
}
