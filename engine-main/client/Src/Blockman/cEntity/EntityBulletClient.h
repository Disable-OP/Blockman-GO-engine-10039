#ifndef __ENTITY_BULLET_CLIENT_HEADER__
#define __ENTITY_BULLET_CLIENT_HEADER__

#include "Entity/EntityBullet.h"
#include "Scene/ModelEntity.h"

namespace BLOCKMAN
{
	class SimpleEffect;

	enum BULLET_TYPE
	{
		BT_NORMAL = 0,
		BT_MORTAR,
		BT_LASER,
		BT_MOMENTARYLASER,
	};

	// bullet of rifle, sniper, and shotgun
	class EntityBulletClient : public EntityBullet
	{
	public:
		SimpleEffect* m_bulletEffect = nullptr;
		String m_bulletEffectName = "";
		BULLET_TYPE m_bulletType = BT_NORMAL;
		bool m_isEffectInit = false;
		EntityLivingBase* m_shootingEntity = nullptr;
		GunSetting* m_gunSetting = nullptr;
		bool m_isFirstPerspective = false;

	protected:
		bool m_needDeleteEffect = true;
		int m_bounceTimes = 0;
		bool m_isShootingByMe = false;

	public:
		EntityBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos, float yawOffset = 0.0f, float pitchOffset = 0.0f);
		~EntityBulletClient();

		virtual void onUpdate() override;
		Vector3 calcPosition(EntityLivingBase* shooting, GunSetting* gunSetting, float rdt = 1.0f);
	};

	// bullet of mortar
	class EntityMortarBulletClient : public EntityBulletClient
	{
	public:
		ModelEntity* m_modelEntity = nullptr;

	public:
		EntityMortarBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos);
		~EntityMortarBulletClient();

		virtual void onUpdate() override;
	};

	// bullet of laserGun, and electricityGun
	class EntityLaserBulletClient : public EntityBulletClient
	{
	public:
		SimpleEffect* m_startEffect = nullptr;
		String m_startEffectName = "";
		Vector3 m_laserBeginPos = Vector3::ZERO;
		Vector3 m_laserEndPos = Vector3::ZERO;
		Vector3 m_lastLaserEndPos = Vector3::ZERO;

	protected:
		float m_contractiveValue = 0.0f;
		int m_currTime = 0;
		int m_lastConsumeBulletTime = 0;
		bool m_canHurt = true;
		int m_record_time = 0;
		int m_sound_type = 0;

	public:
		EntityLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos, float contractiveValue = 0.0f);
		~EntityLaserBulletClient();

		virtual void onUpdate() override;
	};

	// bullet of armor piercing
	class EntityArmorPiercingBulletClient : public EntityBulletClient
	{
	private:
		vector<int>::type hitEntities;

	public:
		EntityArmorPiercingBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos, float yawOffset = 0.0f, float pitchOffset = 0.0f)
			: EntityBulletClient(pWorld, pShooting, pSetting, beginPos, endPos, yawOffset, pitchOffset)
		{}

		virtual void onUpdate() override;
	};

	// bullet of momentary laserGun, and momentary electricityGun
	class EntityMomentaryLaserBulletClient : public EntityLaserBulletClient
	{
	public:
		bool m_hasUsed = false;

	protected:
		int m_existenceCnt = 0;

	public:
		EntityMomentaryLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos);

		virtual void onUpdate() override;
	};

	// bullet of momentary piercing laserGun, and momentary piercing electricityGun
	class EntityMomentaryPiercingLaserBulletClient : public EntityMomentaryLaserBulletClient
	{
	public:
		EntityMomentaryPiercingLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& beginPos, const Vector3& endPos)
			: EntityMomentaryLaserBulletClient(pWorld, pShooting, pSetting, beginPos, endPos)
		{}

		virtual void onUpdate() override;
	};
}

#endif