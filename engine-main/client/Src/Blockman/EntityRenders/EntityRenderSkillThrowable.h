#ifndef __ENTITY_RENDER_SKILL_THROWABLE_HEADER__
#define __ENTITY_RENDER_SKILL_THROWABLE_HEADER__

#include "EntityRender.h"
#include "Entity/EntitySkillThrowable.h"

namespace BLOCKMAN
{
	class AtlasSprite;
	class EntityRenderSkillThrowable : public EntityRender
	{
	private:
		AtlasSprite* sprite = nullptr;
	public:
		EntityRenderSkillThrowable();
		~EntityRenderSkillThrowable();
		/** implement override functions for EntityRender */
		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);

	protected:
		void render_impl(const Vector3& pos, int brightness, int color, AtlasSprite* sprite);

	};

}

#endif