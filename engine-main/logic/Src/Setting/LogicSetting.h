#pragma once

#ifndef __LOGIC_SETTINF_HEADER__
#define __LOGIC_SETTINF_HEADER__

#include "BM_TypeDef.h"
#include "Inventory/InventoryPlayer.h"
#include "GunSetting.h"
#include "BulletClipSetting.h"
#include "MonsterSetting.h"
#include "SkillSetting.h"
#include "GameTypeSetting.h"

namespace BLOCKMAN
{

	class BlockDynamicAttr
	{
	public:
		int blockId;
		float hardness;
		DEFINE_NETWORK_SERIALIZER(blockId, hardness);
	};

	class ToolItemDynamicAttr
	{
	public:
		int itemId;
		float distance;
		float efficiency;
		DEFINE_NETWORK_SERIALIZER(itemId, distance, efficiency);
	};

	class LogicSetting : public Singleton<LogicSetting>, public ObjectAlloc
	{
	private:
		int m_maxInventorySize = InventoryPlayer::MAIN_INVENTORY_COUNT;
		bool m_sneakShowName = false;
		bool m_breakBlockSoon = true;
		bool m_isCanDamageItem = true;
		bool m_isNeedFoodStats = true;
		bool m_isFoodHeal = true;
		float m_attackCoefficientX = 0.04f;
		float parachuteDownSpeed = 0.8f; // = 0.8F;
		float parachuteMoveSpeed = 0.05f; // = 0.05F;
		int m_entityItemLife = 6000;
		float m_disableSelectEntity = false;
		float m_hideClouds = false;
		float m_canCloseChest = true;
		vector<GunPluginSetting>::type m_gunPluginSetting;
		vector<BulletClipPluginSetting>::type m_bulletClipPluginSetting;
		vector<BlockDynamicAttr>::type m_blockDynamicAttr;
		vector<ToolItemDynamicAttr>::type m_toolItemDynamicAttr;
		vector<SkillItem>::type m_skillItemSetting;
		bool m_gunIsNeedBullet = true;
		bool m_showGunEffectWithSingle = false;// single war has no different teamId...it is also useful to the headshot with single war
		bool m_allowHeadshot = false;
		bool m_enableAutoShoot = false;
		float m_thirdPersonDistance = 3.0f;

		ClientGameType m_gameType = ClientGameType::COMMON;
	public:
		LogicSetting();
		~LogicSetting();

		// max inventory size
		int getMaxInventorySize() { return m_maxInventorySize; }
		void setMaxInventorySize(int size) { m_maxInventorySize = size; }

		// sneak show name
		void setSneakShowName(bool isShow) { m_sneakShowName = isShow; }
		bool isSneakShowName() { return m_sneakShowName;  }

		// gun setting
		vector<GunPluginSetting>::type getGunPluginSetting() { return m_gunPluginSetting; }
		void setGunPluginSetting(vector<GunPluginSetting>::type gunPluginSetting);
		void addGunPluginSetting(GunPluginSetting setting);

		//bullet setting
		vector<BulletClipPluginSetting>::type getBulletClipPluginSetting() { return m_bulletClipPluginSetting; }
		void setBulletClipPluginSetting(vector<BulletClipPluginSetting>::type bulletClipPluginSetting);
		void addBulletClipPluginSetting(int bulletId, int maxStack);

		// block dynamic attr
		vector<BlockDynamicAttr>::type getBlockDynamicAttr() { return m_blockDynamicAttr; }
		void setBlockDynamicAttr(vector<BlockDynamicAttr>::type blockDynamicAttr);
		void addBlockDynamicAttr(int blockId, float hardness);

		// tool item dynamic attr
		vector<ToolItemDynamicAttr>::type getToolItemDynamicAttr() { return m_toolItemDynamicAttr; }
		void setToolItemDynamicAttr(vector<ToolItemDynamicAttr>::type toolItemDynamicAttr);
		void addToolItemDynamicAttr(int itemId, float distance, float efficiency);

		// break block soon
		void setBreakBlockSoon(bool isSoon) { m_breakBlockSoon = isSoon; }
		bool isBreakBlockSoon() { return m_breakBlockSoon; }

		// can damage item
		void setCanDamageItem(bool canDamageItem) { m_isCanDamageItem = canDamageItem; }
		bool isCanDamageItem() { return m_isCanDamageItem; }

		// need food stats
		void setNeedFoodStats(bool isNeedFoodStats) { m_isNeedFoodStats = isNeedFoodStats; }
		bool isNeedFoodStats() { return m_isNeedFoodStats; }

		// attack coefficient x
		void setAttackCoefficientX(float x) { m_attackCoefficientX = x; }
		float getAttackCoefficientX() { return m_attackCoefficientX; }

		//Parachute  speed
		float getParachuteDownSpeed() { return parachuteDownSpeed; }
		void setParachuteDownSpeed(float speed) { parachuteDownSpeed = speed; }
		float getParachuteMoveSpeed() { return parachuteMoveSpeed; }
		void setParachuteMoveSpeed(float speed) { parachuteMoveSpeed = speed; }
		bool isDisableSelectEntity() { return m_disableSelectEntity; }
		void setDisableSelectEntity(bool isDisable) { m_disableSelectEntity = isDisable; }

		//isNeedBullet
		void setGunIsNeedBulletStatus(bool isNeedBullet) { m_gunIsNeedBullet = isNeedBullet; }
		bool getGunIsNeedBulletStatus() { return m_gunIsNeedBullet; }

		//monster Setting
		void addMonsterSetting(MonsterSetting setting);
	
		// skill setting
		void addSkillEffect(SkillEffect effect);

		// skillItem setting
		vector<SkillItem>::type getSkillItemSetting() { return m_skillItemSetting; }
		void addSkillItem(SkillItem skillItem);
		void setSkillItemSetting(vector<SkillItem>::type skillItemSetting);
		SkillItem * getSkillItem(int itemId, int Occupation);
		SkillItem * getSkillItem(int skillId);

		//hide clouds
		bool isHideClouds() { return m_hideClouds; }
		void setHideClouds(bool isHide) { this->m_hideClouds = isHide; }

		// food heal
		void setFoodHeal(bool isFoodHeal) { m_isFoodHeal = isFoodHeal; }
		bool isFoodHeal() { return m_isFoodHeal; }

		// entityItem life
		void setEntityItemLife(int life) { m_entityItemLife = life; }
		int getEntityItemLife() { return m_entityItemLife; }

		bool isCanCloseChest() { return m_canCloseChest; }
		void setCanCloseChest(bool closeChest) { this->m_canCloseChest = closeChest; }

		// showGunEffectWithSingle
		void setShowGunEffectWithSingleStatus(bool showGunEffectWithSingle) { m_showGunEffectWithSingle = showGunEffectWithSingle; }
		bool getShowGunEffectWithSingleStatus() { return m_showGunEffectWithSingle; }

		// game type
		void setGameType(ClientGameType gameType) { m_gameType = gameType; }
		ClientGameType getGameType() { return m_gameType; }

		//allow headshot or not
		void setAllowHeadshotStatus(bool isAllowHeadshot) { m_allowHeadshot = isAllowHeadshot; }
		bool getAllowHeadshotStatus() { return m_allowHeadshot; }

		void setAutoShootEnable(bool enable) { m_enableAutoShoot = enable; }
		bool getAutoShootEnable() { return m_enableAutoShoot; }
		
		// set third person distance
		void setThirdPersonDistance(float thirdPersonDistance) { m_thirdPersonDistance = thirdPersonDistance; }
		float getThirdPersonDistance() { return m_thirdPersonDistance; }
	}; 

}

#endif // __LOGIC_SETTINF_HEADER__