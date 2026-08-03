#include "EntityRenderCreatureBullet.h"
#include "EntityRenderManager.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderEntity.h"
#include "Tessolator/BatchRenderable.h"
#include "Tessolator/TessManager.h"
#include "Model/ItemModelMgr.h"
#include "cWorld/Blockman.h"

namespace BLOCKMAN
{
	EntityRenderCreatureBullet::EntityRenderCreatureBullet()
	{
		mAtlasSpriteMap.insert({ CreatureBulletType::Default, TextureAtlasRegister::Instance()->getAtlasSprite("items.json", "fireball") });
		mAtlasSpriteMap.insert({ CreatureBulletType::FireBomb, TextureAtlasRegister::Instance()->getAtlasSprite("items.json", "fireball") });
	}

	EntityRenderCreatureBullet::~EntityRenderCreatureBullet()
	{
		mAtlasSpriteMap.clear();
	}

	void EntityRenderCreatureBullet::doRender(Entity * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		doRenderCreatureBullet((EntityCreatureBullet*)pEntity, pos, yaw, rdt);
	}

	void EntityRenderCreatureBullet::doRenderCreatureBullet(EntityCreatureBullet * pEntity, const Vector3 & pos, float yaw, float rdt)
	{
		map<CreatureBulletType, AtlasSprite*>::iterator iter;

		iter = mAtlasSpriteMap.find(pEntity->getType());

		if (iter == mAtlasSpriteMap.end())
			return;

		AtlasSprite* sprite = iter->second;

		if (!sprite)
			return;

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

		SkinedVertexFmt vertices[4];
		ui32 color = 0xFFFFFFFF;
		vertices[0].setEx2(Vector3(0.f - x1, 0.f - y, 0.f), color, minu, maxv, 0);
		vertices[1].setEx2(Vector3(x - x1, 0.f - y, 0.f), color, maxu, maxv, 0);
		vertices[2].setEx2(Vector3(x - x1, x - y, 0.f), color, maxu, minv, 0);
		vertices[3].setEx2(Vector3(0.f - x1, x - y, 0.f), color, minu, minv, 0);

		int brightUV = pEntity->getBrightnessForRender(0.f);
		int brightness = Blockman::Instance()->m_entityRender->getLightMapColor(brightUV);
		pPackRenderable->addPackMass(vertices, 1, brightness, matBall);
	}
}