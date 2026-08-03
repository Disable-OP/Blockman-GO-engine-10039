#include "EntityRenderBuildNpc.h"
#include "Entity/EntityBuildNpc.h"
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
#include "Setting/BuildingSetting.h"	

namespace BLOCKMAN
{
	EntityRenderBuildNpc::EntityRenderBuildNpc()
	{
	}

	void EntityRenderBuildNpc::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityBuild = dynamic_cast<EntityBuildNpc*>(pEntity);
		if (!entityBuild)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityBuild);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityBuild->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = entityBuild->getBrightnessForRender(rdt);

		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		
		if (entityBuild->m_isDataChange)
		{
			entityBuild->m_isDataChange = false;
			actor->PlaySkill(entityBuild->getAnimate());
		}

		if (entityBuild->m_isActorChange && entityBuild->getActorBody().length() >0 && entityBuild->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(entityBuild->getActorBody(), entityBuild->getActorBodyId());
			entityBuild->m_isActorChange = false;
		}

		position.y += entityBuild->height;
		String name = entityBuild->getName();
		if (name.length() == 0)
		{
			if (entityBuild->getNameLang().length() > 0)
			{
				name = LanguageManager::Instance()->getString(entityBuild->getNameLang());
			}
			else
			{
				name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_SEESION_NAME));
			}
		}

		HeadTextRenderer::renderHeadText(name.c_str(), position, true);
	}
}

