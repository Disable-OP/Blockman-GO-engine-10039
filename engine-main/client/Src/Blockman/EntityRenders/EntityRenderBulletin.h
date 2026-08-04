#ifndef __ENTITY_RENDER_BULLETIN_HEADER__
#define __ENTITY_RENDER_BULLETIN_HEADER__

#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntityRenderBulletin;

	class EntityRenderBulletin : public EntityRender
	{
	public:
		EntityRenderBulletin();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	};
}

#endif // !__ENTITY_RENDER_RANK_HEADER__
