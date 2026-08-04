#include "EntityRenderBullet.h"
#include "EntityRenderManager.h"
#include "bm_clientDef.h"
#include "Model/ItemModelMgr.h"
#include "Entity/EntityBullet.h"
#include "cWorld/Blockman.h"
#include "Render/RenderEntity.h"
#include "Tessolator/BatchRenderable.h"
#include "Tessolator/TessManager.h"
#include "cEntity/EntityBulletClient.h"
#include "World/WorldEffectManager.h"
#include "World/GameSettings.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	EntityRenderBullet::EntityRenderBullet()
	{
	}

	EntityRenderBullet::~EntityRenderBullet()
	{
	}

	void EntityRenderBullet::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		if (!pEntity || pEntity->isDead)
		{
			return;
		}
		EntityBulletClient* bulletEntity = dynamic_cast<EntityBulletClient*>(pEntity);
		if (!bulletEntity)
		{
			return;
		}

		switch (bulletEntity->m_bulletType)
		{
		case BT_NORMAL:
			doRenderNormalBullet(bulletEntity, pos, yaw, rdt);
			break;
		case BT_MORTAR:
			doRenderMortarBullet(bulletEntity, pos, yaw, rdt);
			break;
		case BT_LASER:
			doRenderLaserBullet(bulletEntity, pos, yaw, rdt);
			break;
		case BT_MOMENTARYLASER:
			doRenderMomentaryLaserBullet(bulletEntity, pos, yaw, rdt);
			break;
		default:
			break;
		}
	}

	void EntityRenderBullet::doRenderNormalBullet(EntityBulletClient* pBullet, const Vector3& pos, float yaw, float rdt)
	{
		if (!pBullet->m_isEffectInit)
		{
// 			if (!pBullet->m_shootingEntity || !pBullet->m_gunSetting)
// 			{
// 				return;
// 			}
// 			if (pBullet->m_shootingEntity->isDead)
// 			{
// 				pBullet->m_shootingEntity = nullptr;
// 				return;
// 			}

			//Vector3 startPos = pBullet->calcPosition(pBullet->m_shootingEntity, pBullet->m_gunSetting, rdt);
			Vector3 startPos = pBullet->position;
			pBullet->prevPos = startPos;
			pBullet->position = startPos;
			pBullet->m_bulletEffect = WorldEffectManager::Instance()->addSimpleEffect(pBullet->m_bulletEffectName, startPos, 0.0f, -1);
			pBullet->m_isEffectInit = true;
		}

		if (pBullet->m_bulletEffect)
		{
			Vector3 rdtBulletPos = pBullet->getLastTickPosition(rdt);
			pBullet->m_bulletEffect->mPosition = rdtBulletPos;
		}
	}

	void EntityRenderBullet::doRenderMortarBullet(EntityBulletClient* pBullet, const Vector3& pos, float yaw, float rdt)
	{
		EntityMortarBulletClient* mortarBullet = dynamic_cast<EntityMortarBulletClient*>(pBullet);
		if (!mortarBullet)
		{
			return;
		}

		if (!pBullet->m_isEffectInit)
		{
// 			if (!pBullet->m_shootingEntity || !pBullet->m_gunSetting)
// 			{
// 				return;
// 			}
// 			if (pBullet->m_shootingEntity->isDead)
// 			{
// 				pBullet->m_shootingEntity = nullptr;
// 				return;
// 			}

			//Vector3 startPos = pBullet->calcPosition(pBullet->m_shootingEntity, pBullet->m_gunSetting, rdt);
			Vector3 startPos = pBullet->position;
			pBullet->prevPos = startPos;
			pBullet->position = startPos;
			pBullet->m_bulletEffect = WorldEffectManager::Instance()->addSimpleEffect(pBullet->m_bulletEffectName, startPos, 0.0f, -1, 0.3f, 0.3f, 0.3f);
			pBullet->m_isEffectInit = true;
		}

		Vector3 rdtBulletPos = mortarBullet->getLastTickPosition(rdt);

		if (mortarBullet->m_modelEntity)
		{
			float rdtYaw = mortarBullet->prevRotationYaw + (mortarBullet->rotationYaw - mortarBullet->prevRotationYaw) * rdt;
			float rdtPitch = mortarBullet->prevRotationPitch + (mortarBullet->rotationPitch - mortarBullet->prevRotationPitch) * rdt;
			Quaternion quatYaw = Quaternion(Vector3::UNIT_Y, (rdtYaw - 90.0f) * Math::DEG2RAD);
			Quaternion quatPitch = Quaternion(Vector3::UNIT_Z, rdtPitch * Math::DEG2RAD);
			mortarBullet->m_modelEntity->getSceneNode()->setLocalOrientation(quatYaw * quatPitch);
			mortarBullet->m_modelEntity->getSceneNode()->setWorldPosition(rdtBulletPos + mortarBullet->motion.normalizedCopy() * 0.3f);
			mortarBullet->m_modelEntity->getSceneNode()->update();
			mortarBullet->m_modelEntity->update();
			mortarBullet->m_modelEntity->updateRenderQueue(SceneManager::Instance()->getRenderQueue("Scene"));
		}

		if (pBullet->m_bulletEffect)
		{
			pBullet->m_bulletEffect->mPosition = rdtBulletPos;
		}
	}

	void EntityRenderBullet::doRenderLaserBullet(EntityBulletClient* pBullet, const Vector3& pos, float yaw, float rdt)
	{
		EntityLaserBulletClient* laserBullet = dynamic_cast<EntityLaserBulletClient*>(pBullet);
		if (!laserBullet)
		{
			return;
		}

// 		if (!pBullet->m_shootingEntity || !pBullet->m_gunSetting)
// 		{
// 			return;
// 		}
// 		if (pBullet->m_shootingEntity->isDead)
// 		{
// 			pBullet->m_shootingEntity = nullptr;
// 			return;
// 		}

		if (!pBullet->m_isEffectInit)
		{
			//Vector3 startPos = pBullet->calcPosition(pBullet->m_shootingEntity, pBullet->m_gunSetting, rdt);
			Vector3 startPos = pBullet->position;
			pBullet->m_bulletEffect = WorldEffectManager::Instance()->addSimpleEffect(pBullet->m_bulletEffectName, startPos, 0.0f, -1);
			laserBullet->m_startEffect = WorldEffectManager::Instance()->addSimpleEffect(laserBullet->m_startEffectName, startPos, 0.0f, -1, 0.1f, 0.1f, 0.1f);
			if (laserBullet->m_isFirstPerspective)
			{
				laserBullet->m_bulletEffect->setViewSpace(true);
				laserBullet->m_startEffect->setViewSpace(true);
			}
			pBullet->m_isEffectInit = true;
		}

		Vector3 laserBeginPos = pBullet->getLastTickPosition(rdt);
		Vector3 laserEndPos = laserBullet->m_lastLaserEndPos + (laserBullet->m_laserEndPos - laserBullet->m_lastLaserEndPos) * rdt;

		if (pBullet->m_bulletEffect)
		{
			pBullet->m_bulletEffect->mPosition = laserBeginPos;
			pBullet->m_bulletEffect->setExtenalParam(laserBeginPos, laserEndPos);
			if (laserBullet->m_startEffect)
			{
				laserBullet->m_startEffect->mPosition = laserBeginPos;
			}
		}

	}

	void EntityRenderBullet::doRenderMomentaryLaserBullet(EntityBulletClient* pBullet, const Vector3& pos, float yaw, float rdt)
	{
		EntityMomentaryLaserBulletClient* laserBullet = dynamic_cast<EntityMomentaryLaserBulletClient*>(pBullet);
		if (!laserBullet)
		{
			return;
		}

		if (laserBullet->m_hasUsed && !pBullet->m_isEffectInit)
		{
			pBullet->m_bulletEffect = WorldEffectManager::Instance()->addSimpleEffect(pBullet->m_bulletEffectName, laserBullet->m_laserBeginPos, 0.0f, 580);
			laserBullet->m_startEffect = WorldEffectManager::Instance()->addSimpleEffect(laserBullet->m_startEffectName, laserBullet->m_laserBeginPos, 0.0f, 580, 0.1f, 0.1f, 0.1f);
			if (laserBullet->m_isFirstPerspective)
			{
				laserBullet->m_bulletEffect->setViewSpace(true);
				laserBullet->m_startEffect->setViewSpace(true);
			}
			pBullet->m_isEffectInit = true;

			if (pBullet->m_bulletEffect)
			{
				pBullet->m_bulletEffect->mPosition = laserBullet->m_laserBeginPos;
				pBullet->m_bulletEffect->setExtenalParam(laserBullet->m_laserBeginPos, laserBullet->m_laserEndPos);
				if (laserBullet->m_startEffect)
				{
					laserBullet->m_startEffect->mPosition = laserBullet->m_laserBeginPos;
				}
			}
		}
	}
}