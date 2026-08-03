#include "EntityGunFlame.h"
#include "Object/Root.h"
#include "World/WorldEffectManager.h"
#include "Setting/GunSetting.h"
#include "World/World.h"
#include "Entity/EntityLivingBase.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "cEntity/EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	EntityGunFlame::EntityGunFlame(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, int existenceTime)
		: Entity(pWorld)
	{
		if (!pSetting || !pShooting || pShooting->isDead)
		{
			setDead();
			return;
		}
		m_pShooting = pShooting;
		m_gunSetting = pSetting;
		m_existenceTime = existenceTime;
		m_flameEffectName = pSetting->flameEffect;
		m_currTime = Root::Instance()->getCurrentTime();
		m_beginTime = m_currTime;

		if (pShooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST)
		{
			m_isFirstPerspective = true;
		}

		setPosition(m_pShooting->position);
	}

	EntityGunFlame::~EntityGunFlame()
	{
		if (m_flameEffect)
		{
			m_flameEffect->mDuration = 0;
			m_flameEffect = nullptr;
		}
	}
	void EntityGunFlame::onUpdate()
	{
		m_currTime = Root::Instance()->getCurrentTime();
		if (!Blockman::Instance()->m_isShowGunFlameCoordinate && m_currTime - m_beginTime >= m_existenceTime)
		{
			setDead();
		}

		if (!m_pShooting || m_pShooting->isDead)
		{
			m_pShooting = nullptr;
			setDead();
			return;
		}

		position = calcPosition(m_scale);
		setPosition(position);

		if (m_flameEffect)
		{
			m_flameEffect->mPosition = position;
		}
	}

	Vector3 EntityGunFlame::calcPosition(float& scale)
	{
		bool isFirstView = m_pShooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST;
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Vector3 pos, dir, right, down;
		float frontOff, rightOff, downOff;
		if (isFirstView)
		{
			pos = mainCamera->getPosition();

			dir = mainCamera->getDirection();
			right = mainCamera->getRight();
			down = -mainCamera->getUp();

			frontOff = m_gunSetting->flame_1_frontOff;
			rightOff = m_gunSetting->flame_1_rightOff;
			downOff = m_gunSetting->flame_1_downOff;
			scale = m_gunSetting->flame_1_scale;
		}
		else
		{
			pos = m_pShooting->getPosition();
			pos.y += m_pShooting->getEyeHeight();

			if (m_pShooting == (EntityPlayer*)(Blockman::Instance()->m_pPlayer))
			{
				pos.y += 0.08f;
			}

			float yaw = m_pShooting->renderYawOffset;
			float pitch = 0.f;

			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			right = Vector3::NEG_UNIT_Y.cross(dir);
			down = Vector3::NEG_UNIT_Y;

			frontOff = m_gunSetting->flame_3_frontOff;
			rightOff = m_gunSetting->flame_3_rightOff;
			downOff = m_gunSetting->flame_3_downOff;
			scale = m_gunSetting->flame_3_scale;
		}

		pos += dir * frontOff;
		pos += right * rightOff;
		pos += down * downOff;

		return pos;
	}
}
