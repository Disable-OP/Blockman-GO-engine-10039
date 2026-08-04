#include "EntityRenderSkillThrowable.h"
#include "EntityRenderManager.h"

#include "cItem/cItem.h"
#include "Render/TextureAtlas.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"
#include "Model/ItemModelMgr.h"

#include "Item/Potion.h"
#include "Entity/EntityPotion.h"

#include "Tessolator/BatchRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{

	EntityRenderSkillThrowable::EntityRenderSkillThrowable()
	{
		//sprite = TextureAtlasRegister::Instance()->getAtlasSprite("items.json", "fireball");
	}

	EntityRenderSkillThrowable::~EntityRenderSkillThrowable()
	{
	}

	void EntityRenderSkillThrowable::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		auto entity = dynamic_cast<EntitySkillThrowable*>(pEntity);
		if (entity)
			sprite = TextureAtlasRegister::Instance()->getAtlasSprite(entity->getAtlasName(), entity->getItemIconName());

		if (!sprite)
			return;

		int brightUV = pEntity->getBrightnessForRender(0.f);
				
		if (sprite == cItemPotion::getSpriteByName("bottle_splash"))
		{
			int color = PotionHelper::getColorFromDamage(((EntityPotion*)pEntity)->getPotionDamage(), false);
			color |= 0xFF000000;  // alpha channel.
			render_impl(pos, brightUV, color, cItemPotion::getSpriteByName("overlay"));
		}

		render_impl(pos, brightUV, 0xFFFFFFFF, sprite);
	}

	void EntityRenderSkillThrowable::render_impl(const Vector3& pos, int skyBlockLight, int color, AtlasSprite* sprite)
	{
		Matrix4 matBall = Matrix4::IDENTITY;
		Matrix4 matTemp = Matrix4::IDENTITY;
		matBall.translate(pos);
		matTemp.scale(0.5f, 0.5f, 0.5f);
		matBall.joint(matTemp);
		matTemp.identity();
		matTemp.rotateY(Math::PI - renderManager->playerViewY * Math::DEG2RAD);
		matBall.joint(matTemp);
		matTemp.identity();
		matTemp.rotateX(-renderManager->playerViewX * Math::DEG2RAD);
		matBall.joint(matTemp);

		// get the renderable for skin from the EntityRenderManager and inject the data.
		PackMassRenderable* pPackRenderable = EntityRenderManager::Instance()->getPackRenderable(sprite->getTexture(), SRP_SOLID, 1);

		float minu = sprite->getMinU();
		float maxu = sprite->getMaxU();
		float minv = sprite->getMinV();
		float maxv = sprite->getMaxV();
		float x = 1.0F;
		float x1 = 0.5F;
		float y = 0.25F;
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(skyBlockLight);

		SkinedVertexFmt vertices[4];
		vertices[0].setEx2(Vector3(0.f - x1, 0.f - y, 0.f), brightness, minu, maxv, 0);
		vertices[1].setEx2(Vector3(x - x1, 0.f - y, 0.f), brightness, maxu, maxv, 0);
		vertices[2].setEx2(Vector3(x - x1, x - y, 0.f), brightness, maxu, minv, 0);
		vertices[3].setEx2(Vector3(0.f - x1, x - y, 0.f), brightness, minu, minv, 0);

		Color c = Color(brightness) * Color(color);
		pPackRenderable->addPackMass(vertices, 1, c.getARGB(), matBall);
	}

}
