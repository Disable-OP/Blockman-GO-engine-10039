#pragma once
#include "EntityRenderLiving.h"
#include "Entity/EntityCreature.h"

namespace BLOCKMAN
{
	class EntityCreature;
	class AtlasSprite;

	class EntityRenderCreature : public EntityRenderLiving
	{
	private:
		AtlasSprite* bgBloodStrip = nullptr;
		AtlasSprite* pbBloodStrip = nullptr;

	protected:
		/** render the creature*/
		void renderCreature_impl(EntityCreature* pCreature, const Vector3& pos, float yaw, float ticks);

	public:
		EntityRenderCreature();
		virtual ~EntityRenderCreature();

		/** implement override functions from RenderLiving */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks) override;
		virtual bool compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size) override;
		

	private:
		bool playCreatureAction(EntityCreature * pCreature, ActorObject* actor);
		String getSkillName(EntityCreature* pCreature, CreatureActionState nActionState);
		void playBaseAction(EntityCreature* pCreature, ActorObject* actor);
		void playUpperAction(EntityCreature* pCreature, ActorObject* actor);
		void renderBloodStrip(EntityCreature * pCreature, const Vector3 & pos, float yaw, float rdt);

	};

}
