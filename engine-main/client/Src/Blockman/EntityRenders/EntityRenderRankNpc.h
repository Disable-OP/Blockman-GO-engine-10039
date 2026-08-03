#ifndef __ENTITY_RENDER_RANK_NPC_HEADER__
#define __ENTITY_RENDER_RANK_NPC_HEADER__

#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntityRankNpc;

	class EntityRenderRankNpc : public EntityRender
	{
	public:
		EntityRenderRankNpc();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	};
}

#endif // !__ENTITY_RENDER_RANK_HEADER__
