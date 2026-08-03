#ifndef __ENTITY_RENDER_GRENADE_HEADER__
#define __ENTITY_RENDER_GRENADE_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{
	class cItem;
	class AtlasSprite;

	class EntityRenderGrenade : public EntityRender
	{
	public:
		EntityRenderGrenade();

		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);

	protected:
		void render_impl(const Vector3& pos, int skyBlockLight, int color, AtlasSprite* sprite);

	};

}

#endif
