#ifndef __ENTITY_BLOCKMAN_EMPTY_HEADER__
#define __ENTITY_BLOCKMAN_EMPTY_HEADER__
#include "EntityBlockman.h"
#include "World/World.h"

namespace BLOCKMAN {

	class EntityPlayer;

	enum PlayGuideEffectStatus
	{
		NOT_PLAY,
		HAS_PLAYED,
		HAS_FINISHED,
	};

	class EntityBlockmanEmpty : public EntityBlockman
	{
		RTTI_DECLARE(EntityBlockmanEmpty);

	public:
		int currUpdateTime = 0;
		int lastUpdateTime = 0;
		int lastPushX = 1;
		int lastPushZ = 1;
		float pushX = 0.0f;
		float pushZ = 0.0f;
		bool isCanCollision = true;
		bool isCanLongHit = false;
		int longHitTimes = -1;
		float frozenTime = 0.0f;
		bool outLooksChanged = false;
		PlayGuideEffectStatus m_hasPlayedGuide = NOT_PLAY;

	private:
		multimap<int, Vector2>::type xRecordRoute;
		multimap<int, Vector2>::type zRecordRoute;

		bool isNeedSyncInfo = false;
		bool isNeedSyncMove = false;
		Vector3 lastScriptPos = Vector3::ZERO;

		bool needRecordRoute = false;
		float curSpeed = 0.05f;
		float speedAddition = 0.0f;

	public:
		EntityBlockmanEmpty(World* pWorld);
		EntityBlockmanEmpty(World* pWorld, const Vector3& pos, float yaw, bool isRun);
		
		virtual ~EntityBlockmanEmpty();

		void readEntityFromNBT(NBTTagCompound* pNBT) override {}
		void writeEntityToNBT(NBTTagCompound* pNBT) override {}

		void onUpdate() override;
		bool processInitialInteract(EntityPlayer* par1EntityPlayer) override;

		void onUpdateStart();
		void onUpdateEnd();

		void onFrozenUpdate();
		void onSyncBlockmanInfo();
		void onRecordAllRoute();
		void onRecordRoute(int type);
		bool ifWalkOnRecordRoute();
		bool isMoving();
		
	public:
		void updateOnTrack(int posX, int posY, int posZ, int blockId, int meta) override;
		void applyDrag() override;
		bool checkBlcok(int blockId);
		bool canBeCollidedWith() { return isCanCollision; }

		void reverseDirection();
		void changeDirection(float angle);
		bool canLongHit() { return isCanLongHit; }
		int getLongHitTimes() { return longHitTimes; }
		void setLongHitTimes(int hitTimes) { this->longHitTimes = hitTimes; }
		
		void setNeedRecordRoute(bool needRecord);
		void setCurSpeed(float speed);
		void setSpeedAddtion(float addtion);
		void setNameLang(String nameLang);
		void setActorName(String actorName);
		void setActorBody(String body);
		void setActorBodyId(String bodyId);
		void setCurrentMaxSpeed(float maxSpeed);

		bool isNeedRecordRoute() { return needRecordRoute; }
		float getCurSpeed() { return curSpeed; };
		float getSpeedAddtion() { return speedAddition; }
		float getMoveSpeed();

		bool isFrozen() { return frozenTime > 0.0f; }
		void setOnFrozen(float mills);
		float getFrozenTime() { return frozenTime; }

	};
}
#endif
