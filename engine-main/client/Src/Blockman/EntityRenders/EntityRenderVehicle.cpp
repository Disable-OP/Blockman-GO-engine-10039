#include "EntityRenderVehicle.h"
#include "EntityRenderManager.h"
#include "bm_clientDef.h"

#include "Entity/EntityVehicle.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"

#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorTemplateData.h"
#include "Render/HeadTextRenderer.h"
#include "Setting/CarSetting.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/StringUtil.h"

namespace BLOCKMAN
{
	
EntityRenderVehicle::EntityRenderVehicle()
{
}

EntityRenderVehicle::~EntityRenderVehicle()
{
}

void EntityRenderVehicle::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
{
	auto pVehicle = dynamic_cast<EntityVehicle*>(pEntity);
	if (!pVehicle)
	{
		return;
	}

	ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pVehicle);
	if (!actor || !actor->getHasInited())
		return;

	actor->setBlockManVisible();
	Vector3 position = pVehicle->prevPos + (pVehicle->position - pVehicle->prevPos) * rdt;
	position.y -= pVehicle->yOffset;
	actor->SetPosition(position);
	Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
	actor->SetOrientation(q);
	int brightUV = pVehicle->getBrightnessForRender(rdt);
	int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
	actor->SetBrightness(Color(brightness));
	if (!CarSetting::m_sbIsCarFree)
	{
		const CarSetting* pSetting = pVehicle->getSetting();
		if (pSetting && pSetting->price > 0)
		{
			auto pPlayer = Blockman::Instance()->m_pPlayer;
			if (!pPlayer->hasOwnVehicle(pVehicle->m_vehicleID))
			{
				position.y += 3.0f;
				String price = StringUtil::ToThousandthString(pSetting->price);
				price = StringUtil::Format("Cost: %s", price.c_str());
				HeadTextRenderer::renderHeadText(price, position, true);
			}
		}
	}

	if (pVehicle->m_curState != pVehicle->m_prevState)
	{
		switch (pVehicle->m_curState)
		{
		case VehicleActionState::RUN:
			actor->PlaySkill("run");
			break;
		case VehicleActionState::BACK:
			actor->PlaySkill("back");
			break;
		default:
			actor->PlaySkill("idle");
			break;
		}

		pVehicle->setActionState(pVehicle->m_curState);
	}
}

}
