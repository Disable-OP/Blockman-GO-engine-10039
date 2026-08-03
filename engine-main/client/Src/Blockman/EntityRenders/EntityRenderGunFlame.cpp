#include "EntityRenderGunFlame.h"
#include "cEntity/EntityGunFlame.h"
#include "World/WorldEffectManager.h"
#include "EntityRenderManager.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Setting/GunSetting.h"

#include "Tessolator/BatchRenderable.h"
#include "Tessolator/LineRenderable.h"
#include "Tessolator/TessManager.h"

namespace BLOCKMAN
{
	EntityRenderGunFlame::EntityRenderGunFlame()
	{
		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			m_line = TessManager::Instance()->createLineRenderable(LRP_SELECTED);
		}
	}

	EntityRenderGunFlame::~EntityRenderGunFlame()
	{
		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			TessManager::Instance()->destroyLineRenderable(m_line);
		}
	}

	void EntityRenderGunFlame::doRender(Entity* pEntity, const Vector3& pos, float yaw, float rdt)
	{
		if (!pEntity || pEntity->isDead)
		{
			return;
		}
		EntityGunFlame* gunFlame = dynamic_cast<EntityGunFlame*>(pEntity);
		if (!gunFlame)
		{
			return;
		}

		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			if (!gunFlame->m_pShooting || !gunFlame->m_gunSetting)
			{
				return;
			}
			if (gunFlame->m_pShooting->isDead)
			{
				gunFlame->m_pShooting = nullptr;
				return;
			}

			if (!gunFlame->m_isGunFlameEffectInit)
			{
				if (gunFlame->m_flameEffectName.size())
				{
					float scale = 0.0f;
					Vector3 startPos = calcPosition(gunFlame->m_pShooting, gunFlame->m_gunSetting, rdt, scale);
					gunFlame->m_flameEffect = WorldEffectManager::Instance()->addSimpleEffect(gunFlame->m_flameEffectName, startPos, 0.0f, -1, scale, scale, scale);
					if (gunFlame->m_isFirstPerspective)
					{
						gunFlame->m_flameEffect->setViewSpace(true);
					}
				}
				gunFlame->m_isGunFlameEffectInit = true;
			}

			if (gunFlame->m_flameEffect)
			{
				float scale = 0.0f;
				Vector3 pos = calcPosition(gunFlame->m_pShooting, gunFlame->m_gunSetting, rdt, scale);
				gunFlame->m_flameEffect->mPosition = pos;
				gunFlame->m_flameEffect->mNode->setLocalScaling(Vector3(scale, scale, scale));
			}
		}
		else
		{
			if (!gunFlame->m_isGunFlameEffectInit)
			{
				if (gunFlame->m_flameEffectName.size())
				{
					gunFlame->m_flameEffect = WorldEffectManager::Instance()->addSimpleEffect(gunFlame->m_flameEffectName, gunFlame->position, 0.0f, -1, gunFlame->m_scale, gunFlame->m_scale, gunFlame->m_scale);
					if (gunFlame->m_isFirstPerspective)
					{
						gunFlame->m_flameEffect->setViewSpace(true);
					}
				}
				gunFlame->m_isGunFlameEffectInit = true;
			}

			if (gunFlame->m_flameEffect)
			{
				gunFlame->m_flameEffect->mPosition = gunFlame->position;
				gunFlame->m_flameEffect->mNode->setLocalScaling(Vector3(gunFlame->m_scale, gunFlame->m_scale, gunFlame->m_scale));
			}
		}
	}

	Vector3 EntityRenderGunFlame::calcPosition(EntityLivingBase* shooting, GunSetting* gunSetting, float rdt, float& scale)
	{
		if (!shooting || !gunSetting)
		{
			return Vector3::ZERO;
		}

		bool isFirstView = shooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST;
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Vector3 pos, dir, right, down;
		float frontOff, rightOff, downOff;
		if (isFirstView)
		{
			pos = mainCamera->getPosition();

			dir = mainCamera->getDirection();
			right = mainCamera->getRight();
			down = -mainCamera->getUp();

			frontOff = gunSetting->flame_1_frontOff;
			rightOff = gunSetting->flame_1_rightOff;
			downOff = gunSetting->flame_1_downOff;
			scale = gunSetting->flame_1_scale;
		}
		else
		{
			pos = shooting->getPosition(rdt);
			pos.y += shooting->getEyeHeight();

			if (shooting == (EntityPlayer*)(Blockman::Instance()->m_pPlayer))
			{
				pos.y += 0.08f;
			}

			float yaw = shooting->prevRenderYawOffset + (shooting->renderYawOffset - shooting->prevRenderYawOffset) * rdt;
			float pitch = 0.f;

			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			right = Vector3::NEG_UNIT_Y.cross(dir);
			down = Vector3::NEG_UNIT_Y;

			frontOff = gunSetting->flame_3_frontOff;
			rightOff = gunSetting->flame_3_rightOff;
			downOff = gunSetting->flame_3_downOff;
			scale = gunSetting->flame_3_scale;
		}

		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			std::ifstream infile("scale.txt");
			float flame_1_frontOff, flame_1_rightOff, flame_1_downOff, flame_1_scale;
			float flame_3_frontOff, flame_3_rightOff, flame_3_downOff, flame_3_scale;
			infile >> flame_1_frontOff;
			infile >> flame_1_rightOff;
			infile >> flame_1_downOff;
			infile >> flame_1_scale;
			infile >> flame_3_frontOff;
			infile >> flame_3_rightOff;
			infile >> flame_3_downOff;
			infile >> flame_3_scale;
			infile.close();

			if (isFirstView)
			{
				frontOff = flame_1_frontOff;
				rightOff = flame_1_rightOff;
				downOff = flame_1_downOff;
				scale = flame_1_scale;
			}
			else
			{
				frontOff = flame_3_frontOff;
				rightOff = flame_3_rightOff;
				downOff = flame_3_downOff;
				scale = flame_3_scale;
			}
		}

		pos += dir * frontOff;
		pos += right * rightOff;
		pos += down * downOff;

		if (Blockman::Instance()->m_isShowGunFlameCoordinate)
		{
			Vector3 pointO, pointXP, pointYP, pointZP;
			pointO = dir + shooting->getPosition(rdt) + shooting->getEyeHeight();
			pointXP = pointO + dir;
			pointYP = pointO + right;
			pointZP = pointO + down;
			m_line->beginPolygon();
			LineVertexFmt* points = NULL;
			m_line->getLockPtr_line(2, points);
			points[0].set(pointO.x, pointO.y, pointO.z, 0x000000FF);
			points[1].set(pointXP.x, pointXP.y, pointXP.z, 0x000000FF);
			LineVertexFmt* points2 = NULL;
			m_line->getLockPtr_line(2, points2);
			points2[0].set(pointO.x, pointO.y, pointO.z, 0x00FF0000);
			points2[1].set(pointYP.x, pointYP.y, pointYP.z, 0x00FF0000);
			LineVertexFmt* points3 = NULL;
			m_line->getLockPtr_line(2, points3);
			points3[0].set(pointO.x, pointO.y, pointO.z, 0x0000FF00);
			points3[1].set(pointZP.x, pointZP.y, pointZP.z, 0x0000FF00);
			m_line->endPolygon();
			if (m_line->getPointNum() > 0)
				TessManager::Instance()->addLineRenderable(m_line);
		}

		return pos;
	}
}