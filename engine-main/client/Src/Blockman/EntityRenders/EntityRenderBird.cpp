#include "EntityRenderBird.h"
#include "EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Render/HeadTextRenderer.h"
#include "Model/ModelBiped.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"
#include "Entity/EntityPlayer.h"
#include "World/WorldEffectManager.h"

namespace BLOCKMAN
{

	EntityRenderBird::EntityRenderBird()
		: EntityRenderLiving(LordNew ModelBiped(0.f, 0.f), 0.5f)
	{
	}

	EntityRenderBird::~EntityRenderBird()
	{
	}

	void EntityRenderBird::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float ticks)
	{
		renderBird_impl((EntityBird*)pEntity, pos, yaw, ticks);
	}

	void EntityRenderBird::renderBird_impl(EntityBird * pBird, const Vector3 & pos, float yaw, float rdt)
	{
		if (!pBird)
			return;
		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pBird);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Color color(1.f, 1.f, 1.f, 1.f);
		actor->SetNaturalColor(color, true);
		Vector3 position = pBird->prevPos + (pBird->position - pBird->prevPos) * rdt;
		position.y -= pBird->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = pBird->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		if (pBird->m_isActorChange && pBird->getActorBody().length() > 0 && pBird->getActorBodyId().length() > 0)
		{
			actor->AttachSlave(pBird->getActorBody(), pBird->getActorBodyId());
			actor->AttachSlave("glasses", pBird->getDressGlasses());
			actor->AttachSlave("hat", pBird->getDressHat());
			actor->AttachSlave("beak", pBird->getDressBeak());
			actor->AttachSlave("wing", pBird->getDressWing());
			actor->AttachSlave("tail", pBird->getDressTail());
			actor->AttachSlave("tail_effect", pBird->getDressEffect());
			pBird->m_isActorChange = false;
		}
		playBirdAction(pBird, actor);
		position.y += pBird->getHeight() + 0.2f;
		String name = pBird->getNameLang();
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
			HeadTextRenderer::renderHeadText(name.c_str(), position, true);
		}

		playConvertEffect(pBird, actor);
	}

	bool EntityRenderBird::compileSkinMesh(SkinedRenderable * pRenderable, const Vector2i & pos, const Vector2i & size)
	{
		return false;
	}

	bool EntityRenderBird::playBirdAction(EntityBird * pBird, ActorObject* actor)
	{
		playBaseAction(pBird, actor);
		playUpperAction(pBird, actor);
		return true;
	}

	void EntityRenderBird::playBaseAction(EntityBird* pBird, ActorObject* actor)
	{
		BirdActionState baseAction = pBird->getBaseAction();
		BirdActionState prevBaseAction = pBird->getPrevBaseAction();

		if (prevBaseAction == baseAction)
			return;

		String skillName = getSkillName(pBird, baseAction);
		if (!skillName.empty())
			actor->PlaySkill(skillName);

		pBird->refreshBaseAction();
		pBird->setUpperAction(baseAction);
	}

	void EntityRenderBird::playUpperAction(EntityBird* pBird, ActorObject* actor)
	{
		BirdActionState prevUpperAction = pBird->getPrevUpperAction();
		BirdActionState upperAction = pBird->getUpperAction();
		BirdActionState baseAction = pBird->getBaseAction();

		if (prevUpperAction == upperAction)
			return;

		String skillName = getSkillName(pBird, baseAction);
		if (!skillName.empty())
		{
			actor->PlaySkill(skillName);
		}

		pBird->refreshUpperAction();
	}

	void EntityRenderBird::playConvertEffect(EntityBird * pBird, ActorObject * birdActor)
	{
		EntityPlayer* pPlayer = pBird->findMaster();
		if (!pPlayer || pPlayer->m_bagId == 0 || !pPlayer->m_isBirdConvert || pBird->getBaseAction() != BIRD_AS_CONVERT)
		{
			pBird->destroyConvertEffects();
			return;
		}

		ActorObject* playerActor = EntityRenderManager::Instance()->getEntityActor(pPlayer);
		Vector3 posPlayer = pPlayer->position;
		if (playerActor && playerActor->getHasInited())
		{
			SceneNode* nodePlayer = playerActor->getSocketSceneNode("s_back");
			posPlayer = nodePlayer->getWorldPosition();
		}

		SceneNode* nodeBird = birdActor->getSocketSceneNode("g1041_bird_bone");
		Vector3 posBird = nodeBird->getWorldPosition();
		Vector3 posMid = (posPlayer + posBird) * 0.5f;
		posMid.y += 1.5f;

		if (pBird->m_convertEffects.size() == 0)
		{
			SimpleEffect* effectTemp = WorldEffectManager::Instance()->addSimpleEffect("parabola_fruit.effect", posPlayer, 0.f, -1);
			effectTemp->setParabolicParam(posPlayer - posPlayer, posMid - posPlayer, posBird - posPlayer);
			pBird->m_convertEffects.push_back(effectTemp);
		}
		else
		{
			for (auto item : pBird->m_convertEffects)
			{
				item->setParabolicParam(posPlayer - item->mPosition, posMid - item->mPosition, posBird - item->mPosition);
			}
		}
	}

	String  EntityRenderBird::getSkillName(EntityBird* pBird, const BirdActionState& nActionState)
	{
		String  skillName = "idle";
		switch (nActionState)
		{
		case BIRD_AS_ATTACK:
			skillName = "attack";
			break;
		case BIRD_AS_MOVE:
			skillName = "idle";
			break;
		case BIRD_AS_CONVERT:
			skillName = "conversion";
			break;
		case BIRD_AS_GATHER:
			skillName = "gather";
			break;
		case BIRD_AS_IDLE:
			skillName = "idle";
			break;
		}
		return skillName;
	}
}


