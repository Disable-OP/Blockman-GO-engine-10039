/********************************************************************
filename: 	EntityRenderBullet.h
file path:	dev\client\Src\Blockman\RenderEntity

version:	1
author:		ajohn
company:	supernano
date:		2017-7-3
*********************************************************************/
#ifndef __ENTITY_RENDER_BULLET_HEADER__
#define __ENTITY_RENDER_BULLET_HEADER__

#include "EntityRender.h"

namespace BLOCKMAN
{
	class EntityBulletClient;

	class EntityRenderBullet : public EntityRender
	{
	public:
		EntityRenderBullet();
		~EntityRenderBullet();

		/** implement override functions for EntityRender */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt) override;

	protected:
		void doRenderNormalBullet(EntityBulletClient* pEntityItem, const Vector3& pos, float yaw, float rdt);
		void doRenderMortarBullet(EntityBulletClient* pEntityItem, const Vector3& pos, float yaw, float rdt);
		void doRenderLaserBullet(EntityBulletClient* pEntityItem, const Vector3& pos, float yaw, float rdt);
		void doRenderMomentaryLaserBullet(EntityBulletClient* pEntityItem, const Vector3& pos, float yaw, float rdt);
	};

}

#endif