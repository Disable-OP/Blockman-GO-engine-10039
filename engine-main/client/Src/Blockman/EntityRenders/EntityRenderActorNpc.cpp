#include "EntityRenderActorNpc.h"
#include "Entity/EntityActorNpc.h"
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
	EntityRenderActorNpc::EntityRenderActorNpc()
	{
	}

	void EntityRenderActorNpc::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityActorNpc = dynamic_cast<EntityActorNpc*>(pEntity);
		if (!entityActorNpc)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityActorNpc);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityActorNpc->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = entityActorNpc->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));

		if (!entityActorNpc->getSkillName().empty())
			actor->PlaySkill(entityActorNpc->getSkillName());

		if (entityActorNpc->getHaloEffectName().length() > 0)
		{
			String effectName = entityActorNpc->getHaloEffectName();
			actor->AttachSelectEffect(effectName);
		}
		position.y += entityActorNpc->getHeight() + 0.2f;
		String name = entityActorNpc->getHeadName();
		if (name.length() > 0)
		{
			if (StringUtil::Find(name, "="))
			{
				StringArray kvPair = StringUtil::Split(name, "=");
				name = StringUtil::Format(LanguageManager::Instance()->getString(kvPair[0]).c_str(), kvPair[1].c_str());
			}
			else
			{
				name = LanguageManager::Instance()->getString(name);
			}
			HeadTextRenderer::renderHeadText(name.c_str(), position, entityActorNpc->isCanObstruct());
		}
		if (entityActorNpc->isShowContent())
		{
			position.y -= 1.0f;
			HeadTextRenderer::renderHeadText(entityActorNpc->getContent(), position, true);
		}
	}
}