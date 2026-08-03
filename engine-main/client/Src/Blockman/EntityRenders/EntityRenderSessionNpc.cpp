#include "EntityRenderSessionNpc.h"
#include "Entity/EntitySessionNpc.h"
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
#include "Util/UICommon.h"
#include "Actor/ActorMeshTexModify.h"
namespace BLOCKMAN
{
	EntityRenderSessionNpc::EntityRenderSessionNpc()
	{
	}

	void EntityRenderSessionNpc::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		auto entitySession = dynamic_cast<EntitySessionNpc*>(pEntity);
		if (!entitySession)
		{
			return;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(entitySession);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Vector3 position = pos;
		position.y -= entitySession->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);

		int brightUV = entitySession->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		if (entitySession->m_isActorChange && entitySession->getActorBody().length() >0 && entitySession->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(entitySession->getActorBody(), entitySession->getActorBodyId());
			entitySession->m_isActorChange = false;
			if (entitySession->getActorAction().length() > 0)
			{
				actor->PlaySkill(entitySession->getActorAction());
			}
			//actor->DelMeshTexModify();
			//if (entitySession->getActorBody() == "rank")
			//{
			//	actor->AddMeshTexModify(entitySession->getActorBody(), entitySession->getActorBodyId(), Vector2i(0, 5), Vector2i(174, 18));
			//}
			if (entitySession->getActorBody() == "bagpack")
			{
				actor->DelMeshTexModify();
				actor->AddMeshTexModify(entitySession->getActorBody(), entitySession->getActorBodyId(), Vector2i(1, 214), Vector2i(252, 40));
				actor->getMeshTexModify()->updateString(0.5f, "");
				actor->getMeshTexModify()->updateString(0.f, "");
			}
		}

		if (entitySession->getEffectName().length() > 0)
		{
			String effectName = entitySession->getEffectName();
			actor->AttachSelectEffect(effectName);
		}

		position.y += entitySession->getHeight() + 0.2f;
		String name = entitySession->getName();
		if (name.length() == 0)
		{
			if (entitySession->getNameLang().length() > 0)
			{
				name = LanguageManager::Instance()->getString(entitySession->getNameLang());
			}
		}

		if (entitySession->getTimeLeft() > 1000)
		{
			switch (entitySession->getSessionType())
			{
			case  SessionType::TREASURE_CHEST:
				name = StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_next_refresh_chest").c_str(), UICommon::timeFormat(entitySession->getTimeLeft()).c_str());
				break;
			case  SessionType::BIRD_TASK_TIP:
				name = StringUtil::Format(LanguageManager::Instance()->getString("gui_bird_next_task_time").c_str(), UICommon::timeFormat(entitySession->getTimeLeft()).c_str());
				break;
			}
		}

		HeadTextRenderer::renderHeadText(name.c_str(), position, true);

		//if (entitySession->getActorBody() == "rank")
		//{
		//	ActorMeshTexModify* modifier = actor->getMeshTexModify();
		//	if (modifier)
		//	{
		//		modifier->updateString(0.f, "1111111111", false);
		//	}
		//}
	}

}

