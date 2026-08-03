#include "EntityRenderRankNpc.h"
#include "Entity/EntityRankNpc.h"
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
	EntityRenderRankNpc::EntityRenderRankNpc()
	{
	}

	void EntityRenderRankNpc::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entityRank = dynamic_cast<EntityRankNpc*>(pEntity);
		if (!entityRank)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entityRank);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entityRank->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = entityRank->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));

		position.y += 2.2f;
		String name = entityRank->getName();
		if (name.length() == 0)
		{
			if (entityRank->getNameLang().length() > 0)
			{
				name = LanguageManager::Instance()->getString(entityRank->getNameLang());
			}
			else
			{
				name = LanguageManager::Instance()->getString(LanguageManager::Instance()->getString(LanguageKey::DEFAULT_RANK_NAME));
			}
		}

		HeadTextRenderer::renderHeadText(name.c_str(), position, true);
	}
}

