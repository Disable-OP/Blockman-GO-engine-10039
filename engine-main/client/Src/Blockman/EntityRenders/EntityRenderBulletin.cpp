#include "EntityRenderBulletin.h"
#include "Entity/EntityBulletin.h"
#include "EntityRenders/EntityRenderManager.h"
#include "cWorld/Blockman.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Render/HeadTextRenderer.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Render/RenderEntity.h"


namespace BLOCKMAN
{
	EntityRenderBulletin::EntityRenderBulletin()
	{
	}

	void EntityRenderBulletin::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityBulletin = dynamic_cast<EntityBulletin*>(pEntity);
		if (!entityBulletin)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityBulletin);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityBulletin->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);

		int brightUV = entityBulletin->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		if (entityBulletin->m_isActorChange && entityBulletin->getActorBody().length() >0 && entityBulletin->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(entityBulletin->getActorBody(), entityBulletin->getActorBodyId());
			entityBulletin->m_isActorChange = false;
		}
		position.y += 2.2f;
		String name = entityBulletin->getName();
		
		if (name.length() == 0)
		{
			if (entityBulletin->getNameLang().length() > 0)
			{
				name = LanguageManager::Instance()->getString(entityBulletin->getNameLang());
			}
			else
			{
				name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_SEESION_NAME));
			}
		}

		HeadTextRenderer::renderHeadText(name.c_str(), position, true);
	}

}

