#ifndef __ENTITY_RENDER_BUILD_NPC_HEADER__
#define __ENTITY_RENDER_BUILD_NPC_HEADER__

#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntityBuildNpc;

	class EntityRenderBuildNpc : public EntityRender
	{
	public:
		EntityRenderBuildNpc();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	private:
		bool skillChange = false;
	};
}

#endif // !__ENTITY_RENDER_RANK_HEADER__
