#ifndef __ENTITY_BUILD_NPC_HEADER__
#define __ENTITY_BUILD_NPC_HEADER__
#include "Entity.h"
#include "World/World.h"
#include "Network/protocol/PacketDefine.h"
#include "Setting/RanchSetting.h"

namespace BLOCKMAN {


	enum class BuildType 
	{
		BUILD_DEFAULT = 0,
		BUILD_FARM,
		BUILD_PLANT,
		BUILD_WAREHOUSE
	};

	enum class QueueState
	{
		QUEUE_LOCK = 0,
		QUEUE_LEISURE,
		QUEUE_AWAIT,
		QUEUE_WORKING,
		QUEUE_DONE,
	};

	class ProductQueue
	{
	public:
		i32 queueId = 0;
		i32 productId = 0;
		i32 outputNum = 0;
		i32 timeLeft = 0;//millisecond
		i32 needTime = 1;//millisecond
		i32 unlockPrice = 0;
		i32 currencyType = 0;
		QueueState state = QueueState::QUEUE_AWAIT;
		DEFINE_NETWORK_SERIALIZER(queueId, productId, outputNum, timeLeft, needTime, unlockPrice, currencyType, state);
		ProductQueue() {};
	};

	class ProductRecipe
	{
	public:
		i32 productId = 0;
		i32 needLevel = 0;
		i32 needTime = 1;
		std::vector<RanchCommon> items;
		DEFINE_NETWORK_SERIALIZER(productId, needLevel, needTime, items);
		ProductRecipe() {};
	};

	class EntityBuildNpc : public Entity
	{
		RTTI_DECLARE(EntityBuildNpc);

	private:
		String m_actorName = "rank.actor";
		BuildType m_buildType = BuildType::BUILD_DEFAULT;

		String m_name = "";
		String m_nameLang = "";
		String m_desc = "";
		String m_actorBody = "";
		String m_actorBodyId = "";

		i64 m_frame = 0;
		float m_damage = 0.f;
		bool m_isAwait = false;
		bool m_isWorking = false;
		
	private:
		BlockPos m_startPos;
		BlockPos m_endPos;

	public:
		i32 m_itemId = 0;
		i32 m_actorId = 0;
		ui64 m_userId = 0;
		i32 m_productCapacity = 0;
		i32 m_maxQueueNum = 0;
		String m_awaitAnimate = "";
		String m_manufactureAnimate = "";
		String m_finishAnimate = "";
		vector<ProductRecipe>::type m_recipes;
		vector<RanchCommon>::type m_products;
		vector<ProductQueue>::type m_unlockQueues;
		
		i32 m_queueUnlockPrice = 0;
		i32 m_queueUnlockCurrencyType = 3;
	
	public:
		bool m_isActorChange = true;
		bool m_isDataChange = false;

	public:
		EntityBuildNpc(World* pWorld);
		EntityBuildNpc(World* pWorld, const Vector3& pos);
		~EntityBuildNpc();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}
		void onUpdate() override;
		bool attackEntityFrom(DamageSource* source, float amount) override;
		void singleQueue();
		void syncQueue();

	public:
		String getActorName() const { return m_actorName; }
		BuildType getBuildType() const { return m_buildType; }
		void setBuildType(int type) { m_buildType = (BuildType) type; }
		void setNameLang(const String& nameLang) { m_nameLang = nameLang; }
		String getNameLang() const { return m_nameLang; }
		String getName() const { return m_name; }
		String getDesc()  { return m_desc; }
		void setDesc(String desc) { m_desc = desc; }
		void setName(const String& name) { m_name = name; }
		void setActorName(const String& actorName);
		void setActorBody(String body);
		void setActorBodyId(String bodyId);
		String getActorBody() { return m_actorBody; }
		String getActorBodyId() { return m_actorBodyId; }

		void setActorId(i32 buildId);

		void setRecipes(vector<ProductRecipe>::type& recipes);
		void setProducts(vector<RanchCommon>::type& products);
		void setUnlockQueues(vector<ProductQueue>::type &queues);
		bool canBeCollidedWith() { return true; }

		i32 getProductNum();
		bool isAwait();
		bool isWoring();
		bool isCanWorking() { return isCanWorking(255); }
		bool isCanWorking(i32 blockId);
		bool canMake();
		bool canBuy();
		String getAnimate();

	public:

		bool isImmuneToExplosions() override{ return true; }
	
	};
}
#endif
