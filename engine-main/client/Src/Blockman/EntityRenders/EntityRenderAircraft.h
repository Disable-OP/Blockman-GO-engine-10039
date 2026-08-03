#pragma once
#include "EntityRender.h"

namespace BLOCKMAN
{
	class EntityRenderAircraft : public EntityRender
	{
	public:
		EntityRenderAircraft();
		virtual ~EntityRenderAircraft();

		/** implement override functions for EntityRender */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
	};

}
