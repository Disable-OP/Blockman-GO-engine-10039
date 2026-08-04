#ifndef __ENTITY_RENDER_SESSION_NPC_HEADER__
#define __ENTITY_RENDER_SESSION_NPC_HEADER__

#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntitySessionNpc;

	class EntityRenderSessionNpc : public EntityRender
	{
	public:
		EntityRenderSessionNpc();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	};
}

#endif // !__ENTITY_RENDER_RANK_HEADER__
