#ifndef __ENTITY_RENDER_ACTOR_NPC_HEADER__
#define __ENTITY_RENDER_ACTOR_NPC_HEADER__

#include "EntityRenders/EntityRender.h"
namespace BLOCKMAN
{
	class EntityActorNpc;

	class EntityRenderActorNpc : public EntityRender
	{
	public:
		EntityRenderActorNpc();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	};
}
#endif
