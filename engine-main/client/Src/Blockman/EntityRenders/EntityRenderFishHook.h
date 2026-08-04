#pragma once
#include "EntityRender.h"

namespace BLOCKMAN
{
	class AtlasSprite;

	class EntityRenderFishHook : public EntityRender
	{
	private:
		AtlasSprite* m_sprite = nullptr;
		LineRenderable* m_fishingLine = nullptr;

	public:
		EntityRenderFishHook();
		~EntityRenderFishHook();

		virtual void doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt);

	protected:
		void render_impl(const Vector3& pos, int brightness, int color, AtlasSprite* sprite);
		void drawFishingLine(const Vector3& beginPos, const Vector3& endPos, const Vector3& control1, const Vector3& control2);
	};
}