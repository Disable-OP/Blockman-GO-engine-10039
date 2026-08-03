#ifndef __ENTITY_RENDER_LAND_NPC_HEADER__
#define __ENTITY_RENDER_LAND_NPC_HEADER__

#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntityRenderLandNpc;

	class EntityRenderLandNpc : public EntityRender
	{
	public:
		EntityRenderLandNpc();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	private:
		String timeFormat(i32 time);
	};
}

#endif // !__ENTITY_RENDER_RANK_HEADER__
