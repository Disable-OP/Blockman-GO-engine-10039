#ifndef __ENTITY_LAND_NPC_HEADER__
#define __ENTITY_LAND_NPC_HEADER__

#include "Entity.h"
#include "World/World.h"
#include "Setting/RanchSetting.h"
namespace BLOCKMAN {


	class EntityLandNpc : public Entity
	{
		RTTI_DECLARE(EntityLandNpc);
	private:
		String m_actorName = "rank.actor";

		String m_name = "";
		String m_nameLang = "default.session.name";
		String m_actorBody = "";
		String m_actorBodyId = "";
		ui64 userId = 0;
		i32 status = 0;
		i32 timeLeft = 0;
		i32 landCode = 0;
		i32 totalTime = 1;
		i32 price = 0;

		vector<RanchCommon>::type recipe = vector<RanchCommon>::type();
		vector<RanchCommon>::type reward = vector<RanchCommon>::type();

	
	public:
		bool m_isActorChange = true;

	public:
		EntityLandNpc(World* pWorld);
		EntityLandNpc(World* pWorld, const Vector3& pos);
		~EntityLandNpc();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;

		
		void setNameLang(const String& nameLang) { m_nameLang = nameLang; }
		String getNameLang() const { return m_nameLang; }
		String getName() const { return m_name; }
		void setName(const String& name) { m_name = name; }
		void setActorName(const String& actorName);
		void setActorBody(String body);
		void setActorBodyId(String bodyId);
		String getActorBody() { return m_actorBody; }
		String getActorBodyId() { return m_actorBodyId; }
		String getActorName() const { return m_actorName; }

		const vector<RanchCommon>::type& getRecipe() { return recipe; }
		const vector<RanchCommon>::type& getReward() { return reward; }

		i32 getStatus() { return status; }
		i32 getTimeLeft() { return timeLeft; }
		i32 getLandCode() { return landCode; }
		ui64 getUserId() { return userId; }
		i32 getPrice() { return price; }
		i32 getTotalTime() { return totalTime; }

		void setUserId(ui64 id) { userId = id; }
		void setStatus(i32 s) { status = s; }
		void setLandCode(i32 code) { landCode = code; }
		void setTimeLeft(i32 time) { timeLeft = time; }
		void setPrice(i32 p) { price = p; }
		void setTotalTime(i32 time) { totalTime = time; }

		void setRecipe(const vector<RanchCommon>::type& items);
		void setReward(const vector<RanchCommon>::type& items);

		bool isImmuneToExplosions() override{ return true; }
		bool canBeCollidedWith() { return true; }
	
	};
}
#endif
