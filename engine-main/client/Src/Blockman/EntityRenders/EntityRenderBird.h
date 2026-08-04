#pragma once
#include "EntityRenderLiving.h"
#include "Entity/EntityBird.h"

namespace BLOCKMAN
{
	class EntityBird;

	class EntityRenderBird : public EntityRenderLiving
	{


	public:
		EntityRenderBird();
		virtual ~EntityRenderBird();

		/** implement override functions from RenderLiving */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float ticks) override;
		virtual bool compileSkinMesh(SkinedRenderable* pRenderable, const Vector2i& pos, const Vector2i& size) override; 

	protected:
		/** render the creature*/
		void renderBird_impl(EntityBird* pBird, const Vector3& pos, float yaw, float ticks);

	private:
		bool playBirdAction(EntityBird * pBird, ActorObject* actor);
		void playBaseAction(EntityBird* pBird, ActorObject* actor);
		void playUpperAction(EntityBird* pBird, ActorObject* actor);
		void playConvertEffect(EntityBird* pBird, ActorObject* birdActor);
		String getSkillName(EntityBird* pBird, const BirdActionState& nActionState);


	};

}
