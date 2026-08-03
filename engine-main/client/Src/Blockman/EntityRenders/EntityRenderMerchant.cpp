#include "EntityRenderMerchant.h"
#include "Entity/EntityMerchant.h"
#include "EntityRenders/EntityRenderManager.h"
#include "cWorld/Blockman.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorTemplateData.h"
#include "Anim/AnimBlender.h"
#include "Anim/Skeleton.h"
#include "Anim/Bone.h"
#include "Render/RenderEntity.h"
#include "Render/HeadTextRenderer.h"

namespace BLOCKMAN
{
	EntityRenderMerchant::EntityRenderMerchant()
	{
	}

	void EntityRenderMerchant::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityMerchant = dynamic_cast<EntityMerchant*>(pEntity);
		if (!entityMerchant)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityMerchant);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityMerchant->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = entityMerchant->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		position.y += 2.0f;
		HeadTextRenderer::renderHeadText(entityMerchant->getName(), position, true);
	}
}

