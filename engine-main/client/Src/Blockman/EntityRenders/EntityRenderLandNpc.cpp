#include "EntityRenderLandNpc.h"
#include "Entity/EntityLandNpc.h"
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
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{
	EntityRenderLandNpc::EntityRenderLandNpc()
	{
	}

	void EntityRenderLandNpc::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityLand = dynamic_cast<EntityLandNpc*>(pEntity);
		if (!entityLand)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityLand);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityLand->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);

		
		int brightUV = entityLand->getBrightnessForRender(rdt);
		//Vector3 nightVision = Blockman::Instance()->getNightVisionColor();
		//Color colorBright(brightUV);
		//actor->SetSkyBlockColor(Color(colorBright.r + nightVision.x, colorBright.b + nightVision.y, 0.5f + nightVision.z, 1.f));
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		if (entityLand->m_isActorChange && entityLand->getActorBody().length() >0 && entityLand->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(entityLand->getActorBody(), entityLand->getActorBodyId());
			entityLand->m_isActorChange = false;
		}
		position.y += 2.2f;
		String name = entityLand->getName();
		
		if (name.length() == 0)
		{
			if (entityLand->getNameLang().length() > 0)
			{
				name = LanguageManager::Instance()->getString(entityLand->getNameLang());
			}
			else
			{
				name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_SEESION_NAME));
			}
		}

		if (entityLand->getStatus() == 2)
		{
			String str = LanguageManager::Instance()->getString("gui_ranch_land_npc_name");
			name = StringUtil::Format(str.c_str(), timeFormat(entityLand->getTimeLeft()).c_str());
		}

		HeadTextRenderer::renderHeadText(name.c_str(), position, true);
	}

	String EntityRenderLandNpc::timeFormat(i32 time)
	{
		int h = time / 1000 / 60 / 60;
		int m = time / 1000 / 60 % 60;
		int s = time / 1000 % 60;
		return StringUtil::Format("%d:%d:%d", h, m, s);
	}
}

