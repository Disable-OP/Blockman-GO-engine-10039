#ifndef __ENTITY_RENDER_MERCHANT_HEADER__
#define __ENTITY_RENDER_MERCHANT_HEADER__

#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntityMerchant;

	class EntityRenderMerchant : public EntityRender
	{
	public:
		EntityRenderMerchant();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	};
}

#endif // !__ENTITY_RENDER_MERCHANT_HEADER__
