#pragma once
#include <map>
#include "Core.h"
#include "Item/Items.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;
namespace BLOCKMAN
{
	class GunPluginSetting
	{
	public:
		int gunId;
		int gunType = 0;
		int maxBulletNum = -1;
		float damage = -1;
		float knockback = -1;
		int bulletId = 0;
		float shootRange = 0;
		float cdTime = 0;
		float sniperDistance = -1;
		float mouseSensitivity = 0;
		int bulletPerShoot = -1;
		float bulletOffset = -1;
		float headshotDamage = -1;
		float recoil = 0.0f;
		String bulletEffect = "";
		String flameEffect = "";
		int isPiercing = -1;
		float explosionRange = -1.0f;
		float gravityVelocity = -1.0f;
		int bounceTimes = -1;
		int isAutoShoot = -1;
		DEFINE_NETWORK_SERIALIZER(gunId, gunType, maxBulletNum, damage, knockback, bulletId, shootRange, cdTime, sniperDistance, mouseSensitivity, bulletPerShoot, bulletOffset, headshotDamage, recoil, bulletEffect, flameEffect, isPiercing, explosionRange, gravityVelocity, bounceTimes, isAutoShoot);
	};

	class GunSetting
	{
	public:
		static bool loadSetting(bool bIsClient);
		static void unloadSetting();
		static GunSetting* getGunSetting(int gunId);
		static void updateGunSetting(GunPluginSetting setting);
		static bool isGunItem(int itemId);

		int				gunId;
		int				bulletPerShoot;
		int				maxBulletNum;
		int				soundType;
		int				emptySoundType;
		int				switchClipSoundType;
		int				adaptedBulletId;
		int				reloadTick; // add by zhouyou. reload time ticks(20 ticks = 1 second)

		float			flame_1_frontOff = 0.3f;
		float			flame_1_rightOff = 0.1f;
		float			flame_1_downOff = 0.1f;
		float			flame_1_scale = 1.5f;

		float			flame_3_frontOff = 0.3f;
		float			flame_3_rightOff = 0.1f;
		float			flame_3_downOff = 0.1f;
		float			flame_3_scale = 1.5f;

		GUN_TYPE		gunType;
		BulletType		bulletType;

		float			shootRange;
		float			prepareTime;
		float			cdTime;
		float			sniperDistance;
		float			mouseSensitivity;
		float			bulletOffset;
		float			damage;
		float			bulletSpeed;
		float			knockbackDistance;
		float			recoil;
		float			crossHairMoveSpeed;
		float			crossHairMoveDistance;
		float			headshotDamage;

		String			gunName;
		String			gunDesc;
		String			gunModel;
		String			switchMagazineAction;
		String			previewMesh;
		String			gunIcon;

		String			bulletEffect = "";
		String			flameEffect = "";
		bool			isPiercing = false;
		float			explosionRange = 5.0f;
		float			gravityVelocity = 0.0f;
		int				bounceTimes = 0;
		bool			isAutoShoot = false;

	private:
		static std::map<int, GunSetting*> m_gunSettingMap;
	};
}