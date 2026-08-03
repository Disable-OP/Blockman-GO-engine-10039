#pragma once
#include "EntityRenders/EntityRender.h"

namespace BLOCKMAN
{
	class EntityRenderActorCannon : public EntityRender
	{
	public:
		EntityRenderActorCannon();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;
	};
}