#include "EntityRenderBlockmanEmpty.h"
#include "EntityRenderManager.h"
#include "bm_clientDef.h"

#include "Entity/EntityBlockmanEmpty.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"

#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Actor/ActorTemplateData.h"
#include "Render/HeadTextRenderer.h"
#include "Setting/CarSetting.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/StringUtil.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"

namespace BLOCKMAN
{

	EntityRenderBlockmanEmpty::EntityRenderBlockmanEmpty()
	{
	}

	EntityRenderBlockmanEmpty::~EntityRenderBlockmanEmpty()
	{
	}

	void EntityRenderBlockmanEmpty::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		auto pBlockman = dynamic_cast<EntityBlockmanEmpty*>(pEntity);
		if (!pBlockman)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pBlockman);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pBlockman->prevPos + (pBlockman->position - pBlockman->prevPos) * rdt;
		position.y -= pBlockman->yOffset;
		position.y += 0.4f;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = pBlockman->getBrightnessForRender(rdt);
		Color bright(brightUV);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(bright.b, bright.r);
		actor->SetBrightness(Color(brightness));

		if (pBlockman->m_isActorChange && pBlockman->getActorBody().length() > 0 && pBlockman->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(pBlockman->getActorBody(), pBlockman->getActorBodyId());
			pBlockman->m_isActorChange = false;
		}

		if (pBlockman->outLooksChanged)
		{
			if (pBlockman->isFrozen())
			{
				actor->AddPluginEffect("FrozenEffect", "frozen", "frozen_car.effect");
			}
			else
			{
				actor->DelPluginEffect("FrozenEffect");
			}
			pBlockman->outLooksChanged = false;
		}

		if (pBlockman->isMoving())
			actor->PlaySkill("run");
		else
			actor->PlaySkill("idle");

		switch (pBlockman->m_hasPlayedGuide)
		{
		case BLOCKMAN::NOT_PLAY:
			if (!pBlockman->isMoving())
			{
				actor->DelPluginEffect("CarGuide", false);
				Quaternion quatTemp;
				quatTemp.fromEulerAngle(0.0f, 90.0f, 0.0f);
				actor->AddPluginEffect("CarGuide", "car_guide", "car_guide.effect", 1.0f, 1.0f, -1, Vector3(0.0f, 3.5f, 0.0f), quatTemp);
				pBlockman->m_hasPlayedGuide = HAS_PLAYED;
			}
			break;
		case BLOCKMAN::HAS_PLAYED:
			if (pBlockman->isMoving())
			{
				actor->DelPluginEffect("CarGuide", false);
				pBlockman->m_hasPlayedGuide = HAS_FINISHED;
			}
			break;
		default:
			break;
		}

		position.y += pBlockman->getHeight() + 0.36f;
		String name = pBlockman->getNameLang();
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
			HeadTextRenderer::renderHeadText(name.c_str(), position, false, 1.8f);
		}
	}

}