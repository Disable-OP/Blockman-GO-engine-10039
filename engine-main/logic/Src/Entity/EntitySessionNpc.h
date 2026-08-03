#ifndef __ENTITY_SESSION_NPC_HEADER__
#define __ENTITY_SESSION_NPC_HEADER__
#include "Entity.h"
#include "World/World.h"
namespace BLOCKMAN {

	enum SessionType 
	{
		NONE = -1,
		DEFAULT = 0,
		GET_MANOR = 1,
		SELL_MANOR = 2,
		MANOR_FURNITURE = 3,
		TIP_NPC = 4,
		MULTI_TIP_NPC = 5,
		PERSONAL_SHOP = 6,
		STATUE_NPC = 7,
		SUPER_SHOP = 8,
		MULTI_TIP_INTERACTION_NPC = 9,
		RANCHERS_EXPLORE_BEGIN = 10,
		RANCHERS_TASK_FINISH = 11,
		ENCHANTMENT = 12,
		MODE_SELECT = 13,
		GUN_STORE = 14,
		CHEST_LOTTERY = 15,
		ARMOR_UPGRADE = 16,
		SEASON_RANK = 17,
		TREASURE_CHEST = 104101,
		BIRD_LOTTERY = 104102,
		BIRD_STORE_ITEM = 104103,
		BIRD_TASK_TIP = 104104,
	};

	class EntitySessionNpc : public Entity
	{
		RTTI_DECLARE(EntitySessionNpc);
	protected:
		bool m_canCollided = true;
		bool m_isPerson = false;
		SessionType m_sessionType = SessionType::DEFAULT;
		i32 m_timeLeft = 0;
		String m_name = "";
		String m_actorName = "";
		String m_nameLang = "";
		String m_actorBody = "";
		String m_actorBodyId = "";
		String m_actorAction = "";
		String m_effectName = "";
		String m_sessionContent = "";
		
		map<ui64, i32>::type m_timeLeftMap;
		map<ui64, String>::type  m_nameLangMap;
		map<ui64, String>::type  m_actorNameMap;
		map<ui64, String>::type  m_actorBodyMap;
		map<ui64, String>::type  m_actorBodyIdMap;
		map<ui64, String>::type  m_effectNameMap;
		map<ui64, String>::type  m_sessionContentMap;
		map<ui64, String>::type  m_actorActionMap;
		map<ui64, bool>::type  m_canCollidedMap;

	public:
		bool m_isActorChange = true;

	public:
		EntitySessionNpc(World* pWorld);
		EntitySessionNpc(World* pWorld, const Vector3& pos);
		~EntitySessionNpc();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;
		bool canBeCollidedWith() { return m_canCollided; }

		void setActorName(const String& actorName);
		String getActorName() const { return m_actorName; }
		SessionType getSessionType() const { return m_sessionType; }
		void setSessionType(int type) { m_sessionType = (SessionType)type; }
		void setSessionContent(const String& content) { this->m_sessionContent = content; }
		String getSessionContent() { return m_sessionContent; }
		void setNameLang(const String& nameLang) { m_nameLang = nameLang; }
		String getNameLang() const { return m_nameLang; }
		String getName() const { return m_name; }
		void setName(const String& name) { m_name = name; }
		void setEffectName(const String & effectName) { m_effectName = effectName; }
		void setTimeLeft(i32 time) { m_timeLeft = time; }

		void setActorBody(String body);
		void setActorBodyId(String bodyId);
		void setActorAction(String action);
		String getActorBody() { return m_actorBody; }
		String getActorAction() { return m_actorAction; }
		String getActorBodyId() { return m_actorBodyId; }
		String getEffectName() { return m_effectName; }
		i32 getTimeLeft() { return m_timeLeft; }

		void setPerson(bool isPerson) { m_isPerson = isPerson; }
		bool isPerson() { return m_isPerson; }

		void setTimeLeftByUser(ui64 userId, i32 timeLeft);
		void setNameLangByUser(ui64 userId, String nameLang);
		void setActorNameByUser(ui64 userId, String actorName);
		void setActorBodyByUser(ui64 userId, String actorBody);
		void setActorBodyIdByUser(ui64 userId, String actorBodyId);
		void setEffectNameByUser(ui64 userId, String effectName);
		void setSessionContentByUser(ui64 userId, String content);
		void setActorActionByUser(ui64 userId, String ctorAction);
		void setCanCollidedByUser(ui64 userId, bool isCollided);

		i32 getTimeLeftByUser(ui64 userId);
		String getNameLangByUser(ui64 userId);
		String getActorNameByUser(ui64 userId);
		String getActorBodyByUser(ui64 userId);
		String getActorBodyIdByUser(ui64 userId);
		String getActorActionByUser(ui64 userId);
		String getEffectNameByUser(ui64 userId);
		String getSessionContentByUser(ui64 userId);
		bool isCanCollidedByUser(ui64 userId);

		bool isImmuneToExplosions() override{ return true; }
		void setCanCollided(bool canCollided) { m_canCollided = canCollided; }

	private:
		i64 m_frame = 0;
		void removeInexistencePlayerData();
	
	};
}
#endif
