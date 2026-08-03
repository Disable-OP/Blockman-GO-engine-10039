#include "EntityRenderFishHook.h"
#include "EntityRenderManager.h"

#include "Render/TextureAtlas.h"
#include "Render/RenderEntity.h"
#include "Model/ItemModelMgr.h"
#include "cWorld/Blockman.h"

#include "cEntity/EntityFishHookClient.h"
#include "Entity/EntityLivingBase.h"
#include "Entity/EntityPlayer.h"
#include "cEntity/EntityPlayerSPMP.h"

#include "Tessolator/BatchRenderable.h"
#include "Tessolator/LineRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{
	EntityRenderFishHook::EntityRenderFishHook()
	{
		m_fishingLine = TessManager::Instance()->createLineRenderable(LRP_SELECTED);
	}

	EntityRenderFishHook::~EntityRenderFishHook()
	{
		TessManager::Instance()->destroyLineRenderable(m_fishingLine);
	}

	void EntityRenderFishHook::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		auto entity = dynamic_cast<EntityFishHookClient*>(pEntity);

		if (!entity)
		{
			return;
		}

		m_sprite = TextureAtlasRegister::Instance()->getAtlasSprite("items.json", "fishing_hook");

		if (!m_sprite)
			return;

		int brightUV = pEntity->getBrightnessForRender(0.f);

		render_impl(pos, brightUV, 0xFFFFFFFF, m_sprite);

		// draw fishingLine
		Vector3 beginPoint;
		Vector3 endPoint = pos;
		endPoint.y += 0.35f;

		bool drawLine = true;
		EntityPlayer* pPlayer = dynamic_cast<EntityPlayer*>(entity->getAngler());
		if (pPlayer == Blockman::Instance()->m_pPlayer && !Blockman::Instance()->getCurrPersonView())
		{
			Vector3 offset(0.57f, 0.88f, 0.0f);
			Camera* mainCam = SceneManager::Instance()->getMainCamera();
			Matrix4 invViewMatrix = mainCam->getViewMatrix();
			Matrix4 viewBobing = TessManager::Instance()->getViewBobing();
			invViewMatrix.detInverse();
			
			Vector3 fishPoint_view = offset * viewBobing;
			fishPoint_view = fishPoint_view * Blockman::Instance()->m_firstPersonItemMat;
			Vector3 fishPoint_world = fishPoint_view * invViewMatrix;
			//Vector3 fishPoint_world = offset * temp;
			beginPoint = fishPoint_world;
		}
		else if (pPlayer && entity->m_thirdWorldMat != Matrix4::IDENTITY)
		{
			Vector3 offset(0.08f, 0.05f, 1.06f);
			Matrix4 worldMat = entity->m_thirdWorldMat;
			Vector3 fishPoint_world = offset * worldMat;
			beginPoint = fishPoint_world;
		}
		else if (pPlayer)
		{
			beginPoint = pPlayer->position;
		}
		else
		{
			drawLine = false;
		}

		if (drawLine)
		{
			Vector3 control1 = beginPoint * 0.2f + endPoint * 0.8f;
			float deltaHeightY = endPoint.y - control1.y;
			control1.y += deltaHeightY;
			Vector3 control2 = beginPoint * 0.5f + endPoint * 0.5f;
			control2.y += deltaHeightY;

			drawFishingLine(beginPoint, endPoint, control2, control1);

			if (m_fishingLine->getPointNum() > 0)
				TessManager::Instance()->addLineRenderable(m_fishingLine);
		}
	}

	void EntityRenderFishHook::render_impl(const Vector3& pos, int skyBlockLight, int color, AtlasSprite* sprite)
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

	void EntityRenderFishHook::drawFishingLine(const Vector3& beginPos, const Vector3& endPos, const Vector3& control1, const Vector3& control2)
	{
		m_fishingLine->beginPolygon();

		int pointsNum = 50;
		LineVertexFmt* points = NULL;
		m_fishingLine->getLockPtr_line(pointsNum + 1, points);

		float f1, f2, f3, temp;
		float delta = 1.0f / pointsNum;
		for (int i = 0; i <= pointsNum; i++)
		{
			temp = i * delta;
			f1 = (1 - temp) * (1 - temp);
			f2 = 2 * temp * (1 - temp);
			f3 = temp * temp;
			float x = f1 * beginPos.x + f2 * control1.x + f3 * endPos.x;
			float y = f1 * beginPos.y + f2 * control1.y + f3 * endPos.y;
			float z = f1 * beginPos.z + f2 * control1.z + f3 * endPos.z;
			points[i].set(x, y, z, 0xFF000000);
		}
		m_fishingLine->endPolygon();
	}
}