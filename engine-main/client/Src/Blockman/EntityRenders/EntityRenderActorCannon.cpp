#include "EntityRenderActorCannon.h"
#include "cEntity/EntityActorCannonClient.h"
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
	EntityRenderActorCannon::EntityRenderActorCannon()
	{
	}

	void EntityRenderActorCannon::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityActorCannon = dynamic_cast<EntityActorCannonClient*>(pEntity);
		if (!entityActorCannon)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityActorCannon);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityActorCannon->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = entityActorCannon->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));

		if (entityActorCannon->m_hasBeenLaunched)
		{
			actor->DelPluginEffect("CannonFireEffect", false);

			Quaternion quatTemp;
			quatTemp.fromEulerAngle(60.0f, -40.0f, 0.0f);

			actor->AddPluginEffect("CannonFireEffect", "cannon_fire", "cannon_fire.effect", 1.0f, 1.0f, 1, Vector3(0.0f, 1.6f, 2.0f), quatTemp);
			entityActorCannon->m_hasBeenLaunched = false;
		}

		if (!entityActorCannon->getSkillName().empty())
			actor->PlaySkill(entityActorCannon->getSkillName());

		if (entityActorCannon->getHaloEffectName().length() > 0)
		{
			String effectName = entityActorCannon->getHaloEffectName();
			actor->AttachSelectEffect(effectName);
		}
		position.y += entityActorCannon->getHeight() + 0.2f;
		String name = entityActorCannon->getHeadName();
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
			HeadTextRenderer::renderHeadText(name.c_str(), position, entityActorCannon->isCanObstruct());
		}
		if (entityActorCannon->isShowContent())
		{
			position.y -= 1.0f;
			HeadTextRenderer::renderHeadText(entityActorCannon->getContent(), position, true);
		}
	}
}