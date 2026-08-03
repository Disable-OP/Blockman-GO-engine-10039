#include "EntityRenderCreature.h"
#include "EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Actor/ActorManager.h"
#include "Render/HeadTextRenderer.h"
#include "Model/ModelBiped.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"
#include "Render/TextureAtlas.h"
#include "Model/ItemModelMgr.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/GameRuleSetting.h"

namespace BLOCKMAN
{

	EntityRenderCreature::EntityRenderCreature()
		: EntityRenderLiving(LordNew ModelBiped(0.f, 0.f), 0.5f)
	{
		bgBloodStrip = TextureAtlasRegister::Instance()->getAtlasSprite("blood_strip.json", "bg_blood_strip");
		pbBloodStrip = TextureAtlasRegister::Instance()->getAtlasSprite("blood_strip.json", "pb_blood_strip");
	}

	EntityRenderCreature::~EntityRenderCreature()
	{
	}

	void EntityRenderCreature::renderCreature_impl(EntityCreature * pCreature, const Vector3 & pos, float yaw, float rdt)
	{
		if (!pCreature)
		{
			return;
		}
		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(pCreature);
		if (!actor || !actor->getHasInited())
			return;

		actor->setBlockManVisible();
		Color color(1.f, 1.f, 1.f, 1.f);
		if (pCreature->isHurt())
		{
			color.set(1.f, 0.4f, 0.f, 1.f);
		}
		actor->SetNaturalColor(color, true);
		Vector3 position = pCreature->prevPos + (pCreature->position - pCreature->prevPos) * rdt;
		position.y -= pCreature->yOffset;
		actor->SetPosition(position);
		Quaternion q(Vector3::UNIT_Y, -yaw * Math::DEG2RAD);
		actor->SetOrientation(q);
		int brightUV = pCreature->getBrightnessForRender(rdt);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		actor->SetBrightness(Color(brightness));
		actor->AttachSlave("body", StringUtil::ToString(1));
		playCreatureAction(pCreature, actor);
		position.y += pCreature->getHeight() + 0.2f;
		String name = pCreature->getNameLang();
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

		renderBloodStrip(pCreature, pos, yaw, rdt);
	}

	void EntityRenderCreature::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float ticks)
	{
		renderCreature_impl((EntityCreature*)pEntity, pos, yaw, ticks);
	}

	bool EntityRenderCreature::compileSkinMesh(SkinedRenderable * pRenderable, const Vector2i & pos, const Vector2i & size)
	{
		return false;
	}

	bool EntityRenderCreature::playCreatureAction(EntityCreature * pCreature, ActorObject* actor)
	{
		playBaseAction(pCreature, actor);
		playUpperAction(pCreature, actor);
		return true;
	}

	void EntityRenderCreature::playBaseAction(EntityCreature* pCreature, ActorObject* actor)
	{
		CreatureActionState baseAction = pCreature->getBaseAction();
		CreatureActionState prevBaseAction = pCreature->getPrevBaseAction();

		if (prevBaseAction == baseAction)
			return;

		String skillName = getSkillName(pCreature, baseAction);
		if (!skillName.empty())
			actor->PlaySkill(skillName);

		pCreature->refreshBaseAction();
		pCreature->setUpperAction(baseAction);
	}

	void EntityRenderCreature::playUpperAction(EntityCreature* pCreature, ActorObject* actor)
	{
		CreatureActionState prevUpperAction = pCreature->getPrevUpperAction();
		CreatureActionState upperAction = pCreature->getUpperAction();
		CreatureActionState baseAction = pCreature->getBaseAction();

		if (prevUpperAction == upperAction)
			return;

		String skillName = getSkillName(pCreature, baseAction);
		if (!skillName.empty())
		{
			actor->PlaySkill(skillName);
		}

		pCreature->refreshUpperAction();
	}

	String  EntityRenderCreature::getSkillName(EntityCreature* pCreature, CreatureActionState nActionState)
	{
		String  SkillName = "idle";
		switch (nActionState)
		{
		case CREATURE_AS_ATTACK:
			if(pCreature->isBoss())
				SkillName = "attack2";
			else if (pCreature->isMonster())
				SkillName = "attack";
			break;
		case CREATURE_AS_BE_OTHER_ATTACK:
			break;
		case CREATURE_AS_MOVE:
			SkillName = "walk";
			break;
		case CREATURE_AS_DEATH:
			SkillName = "hitdown";
			break;
		default:
			break;
		}
		return SkillName;
	}

	void EntityRenderCreature::renderBloodStrip(EntityCreature * pCreature, const Vector3 & pos, float yaw, float rdt)
	{

		auto ruleSetting = GameRuleSetting::getGameRule();
		if (bgBloodStrip == NULL || pbBloodStrip == NULL || !ruleSetting->isHaveLifebar)
			return;

		EntityPlayer* player = Blockman::Instance()->m_pPlayer;
		if (!player)
			return;
		float playerPitch = player->rotationPitch * Math::DEG2RAD;
		float playerYaw = player->rotationYaw * Math::DEG2RAD;

		Vector3 position = pCreature->getPosition(rdt, true);
		position.y += pCreature->getHeight() + 1.3F - pCreature->yOffset;

		int skyBlockLight = pCreature->getBrightnessForRender(0.f);
		int color = 0xFFFFFFFF;

		Matrix4 matBall, matTemp;
		matBall.identity();
		matBall.translate(position);
		matTemp.identity();
		matTemp.rotateY(-playerYaw + Math::PI);
		matBall.joint(matTemp);
		matTemp.identity();
		matTemp.rotateX(-playerPitch);
		matBall.joint(matTemp);

		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(skyBlockLight);
		Color c = Color(brightness) * Color(color);

		PackMassRenderable* pbPackRenderable = EntityRenderManager::Instance()->getPackRenderable(pbBloodStrip->getTexture(), SRP_SOLID, 1);
		float pbminu = pbBloodStrip->getMinU();
		float pbmaxu = pbBloodStrip->getMaxU();
		float pbminv = pbBloodStrip->getMinV();
		float pbmaxv = pbBloodStrip->getMaxV();

		float pbscaley = (float)(pbBloodStrip->getFrameSize().y) / (float)(pbBloodStrip->getFrameSize().x) / 2;
		float hp_percent = pCreature->getHealth() / pCreature->getMaxHealth();
		float subpbx = 1.0f - (pCreature->getHealth() / pCreature->getMaxHealth());
		SkinedVertexFmt pbvertices[4];
		pbvertices[0].setEx2(Vector3(-0.5f, -pbscaley, 0.f), brightness, pbminu, pbmaxv, 0);
		pbvertices[1].setEx2(Vector3(0.5f - subpbx, -pbscaley, 0.f), brightness, pbmaxu * hp_percent, pbmaxv, 0);
		pbvertices[2].setEx2(Vector3(0.5f - subpbx, pbscaley, 0.f), brightness, pbmaxu * hp_percent, pbminv, 0);
		pbvertices[3].setEx2(Vector3(-0.5f, pbscaley, 0.f), brightness, pbminu, pbminv, 0);
		pbPackRenderable->addPackMass(pbvertices, 1, c.getARGB(), matBall);

		PackMassRenderable* bgPackRenderable = EntityRenderManager::Instance()->getPackRenderable(bgBloodStrip->getTexture(), SRP_SOLID, 1);
		float bgminu = bgBloodStrip->getMinU();
		float bgmaxu = bgBloodStrip->getMaxU();
		float bgminv = bgBloodStrip->getMinV();
		float bgmaxv = bgBloodStrip->getMaxV();

		float bgscaley = (float)(bgBloodStrip->getFrameSize().y) / (float)(bgBloodStrip->getFrameSize().x) / 2;

		SkinedVertexFmt bgvertices[4];
		bgvertices[0].setEx2(Vector3(-0.5f, -bgscaley, 0.f), brightness, bgminu, bgmaxv, 0);
		bgvertices[1].setEx2(Vector3(0.5f, -bgscaley, 0.f), brightness, bgmaxu, bgmaxv, 0);
		bgvertices[2].setEx2(Vector3(0.5f, bgscaley, 0.f), brightness, bgmaxu, bgminv, 0);
		bgvertices[3].setEx2(Vector3(-0.5f, bgscaley, 0.f), brightness, bgminu, bgminv, 0);
		bgPackRenderable->addPackMass(bgvertices, 1, c.getARGB(), matBall);
	}
}


