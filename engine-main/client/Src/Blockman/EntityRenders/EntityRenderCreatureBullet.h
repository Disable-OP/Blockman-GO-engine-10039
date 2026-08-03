#ifndef __ENTITY_RENDER_CREATURE_BULLET_HEADER__
#define __ENTITY_RENDER_CREATURE_BULLET_HEADER__

#include "EntityRender.h"
#include "Entity/EntityCreatureBullet.h"

namespace BLOCKMAN
{
	class AtlasSprite;

	class EntityRenderCreatureBullet : public EntityRender
	{
	private:
		std::map<CreatureBulletType, AtlasSprite*> mAtlasSpriteMap;
	public:
		EntityRenderCreatureBullet();
		~EntityRenderCreatureBullet();
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);
	protected:
		void doRenderCreatureBullet(EntityCreatureBullet* pEntity, const Vector3& pos, float yaw, float rdt);

	private:

	};

}

#endif