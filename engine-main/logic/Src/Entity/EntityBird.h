/********************************************************************
filename: 	EntityBird.h
file path:	dev\client\Src\Blockman\Entity

version:	1
author:		ajohn
company:	supernano
date:		2017-3-3
*********************************************************************/
#ifndef __ENTITY_BIRD_HEADER__
#define __ENTITY_BIRD_HEADER__

#include "EntityLiving.h"
#include "Network/protocol/C2SPackets.h"
#include <mutex>

namespace BLOCKMAN
{

	class PathEntity;
	class AIBase;
	class SimpleEffect;

	enum BirdActionState
	{
		BIRD_AS_IDLE = 0,
		BIRD_AS_MOVE = 1,
		BIRD_AS_GATHER = 2,
		BIRD_AS_ATTACK  = 3,
		BIRD_AS_CONVERT =4,
	};

	class EntityBird : public EntityLiving
	{
	public:
		RTTI_DECLARE(EntityBird);

		bool m_isActorChange = false;

		i64 m_birdId = 0;
		String m_nameLang = "";
		String m_actorName = "";
		String m_actorBody = "";
		String m_actorBodyId = "";

		String m_dressGlasses = "";
		String m_dressHat = "";
		String m_dressBeak = "";
		String m_dressWing = "";
		String m_dressTail = "";
		String m_dressEffect = "";

		ui64 m_userId = 0;

		bool isEntityAI = true;
		int m_curUpdateTime = 0;
		int m_lastUpdateTime = 0;
		float m_attackCdTime = 0;
		float m_freeSkillCdTime = 0;
		float m_minPlayerDistance = 20.0f;
		BirdActionState m_prevState;
		BirdActionState m_curState;

		int m_targetEntityId = 0;
		bool m_revengingTarget = false;

		BirdActionState m_prevBaseAction;
		BirdActionState m_prevUpperAction;
		Listenable<BirdActionState> m_baseAction;
		Listenable<BirdActionState> m_upperAction;

		Vector3 m_masterPosition = Vector3::INVALID;

		Vector3 m_forcMovePos = Vector3::INVALID;
		float m_forceYaw = FLT_MAX;

		vector<SimpleEffect*>::type m_convertEffects;

	protected:
		float m_updateTargetCdTime = 0;
		float m_updatePathCdTime = 0;
		int m_pathTargetId = 0;



	public:
		virtual void forceMoveTo(Vector3 toPos);
		Vector3 getForceMoveTo() { return m_forcMovePos; }

		void forceRotation(float yaw);
		float getForceYaw() { return m_forceYaw; }

		BirdActionState getPrevBaseAction() { return m_prevBaseAction; }
		BirdActionState getPrevUpperAction() { return m_prevUpperAction; }
		BirdActionState getBaseAction() { return m_baseAction; }
		void setBaseAction(BirdActionState as) { m_prevBaseAction = m_baseAction; m_baseAction = as; }
		BirdActionState getUpperAction() { return m_upperAction; }
		void setUpperAction(BirdActionState as) { m_prevUpperAction = m_upperAction; m_upperAction = as; }
		void refreshBaseAction() { m_prevBaseAction = m_baseAction; }
		void refreshUpperAction() { m_prevUpperAction = m_upperAction; }

		void changeDress(const String & masterName, const String & slaveName);

		void setActorName(const String& actorName);
		void setActorBody(const String& actorBody);
		void setActorBodyId(const String& actorBodyId);
		void setDressGlasses(const String& dressGlasses);
		void setDressHat(const String& dressHat);
		void setDressBeak(const String& dressBeak);
		void setDressWing(const String& dressWing);
		void setDressTail(const String& dressTail);
		void setDressEffect(const String& dressEffect);

		String getActorName() const { return m_actorName; }
		String getActorBody() const { return m_actorBody; }
		String getActorBodyId() const { return m_actorBodyId; }
		String getDressGlasses() const { return m_dressGlasses; }
		String getDressHat() const { return m_dressHat; }
		String getDressBeak() const { return m_dressBeak; }
		String getDressWing() const { return m_dressWing; }
		String getDressTail() const { return m_dressTail; }
		String getDressEffect() const { return m_dressEffect; }

		

		void setNameLang(const String& nameLang)
		{
			m_nameLang = nameLang;
		}

		String getNameLang() const
		{
			return m_nameLang;
		}

		virtual bool changeBirdAction(BirdActionState nActionState);
		virtual BirdActionState getBirdAction() { return m_curState; }
		virtual BirdActionState getPreBirdAction() { return m_prevState; }

		virtual void setCurrentTargetEntityId(int EntityId, bool revengingTarget);
		virtual int getCurrentTargetEntityId() { return m_targetEntityId; }
		virtual bool isRevengingTarget() { return m_revengingTarget; }
		virtual bool entityInAttackRange(Entity* entity);

		virtual void setActionState(BirdActionState state) { m_prevState = m_curState; m_curState = state; }
		virtual void setSpeedAdditionLevel(int level);

		virtual void setSmallAttackCd();
		virtual void setAttackCd();
		virtual float getAttackCd();
		virtual void refreshAttackCdTime();

		virtual void setFreeSkillCd();
		virtual float getFreeSkillCd();
		virtual void refreshFreeSkillCdTime();

		virtual void setUpdateTargetCd();
		virtual float getUpdateTargetCd();
		virtual void refreshUpdateTargetCdTime();


		virtual void setUpdatePathCd(int pathTargetId);
		virtual float getUpdatePathCd(int pathTargetId);
		virtual void refreshUpdatePathCdTime();

		virtual int getAttackCount();

		virtual int getBirdAttackType();
		virtual void attackPerformance(EntityLivingBase* targeter);

		virtual EntityPlayer* findMaster();

		virtual float getFlySpeed() { return 0.2f; };

		bool isImmuneToExplosions() { return true; }
		void destroyConvertEffects();

	protected:

		PathEntity* pathToEntity = nullptr;

		/** The Entity this EntityBird is set to attack. */
		Entity* entityToAttack = nullptr;

		/** returns true if a creature has attacked recently only used for creepers and skeletons */
		bool hasAttacked = false;

		/** Used to make a creature speed up and wander away when hit. */
		int fleeingTick = 0;

		BlockPos homePosition; // = new ChunkCoordinates(0, 0, 0);

		/** If -1 there is no maximum distance */
		float maximumHomeDistance = -1.f; // = -1.0F;
		AIBase* m_aiMoveTo = nullptr; // = new EntityAIMoveTowardsRestriction(this, 1.0D);
		bool m_hasTasks = false;

		float moveUpDown = 0.0f;

		bool m_bNeedRegMovePath = false;

		std::vector<BlockPos> m_movePath;

	public:
		EntityBird(World* pWorld);
		EntityBird(World* pWorld, i64 birdId, Vector3& pos, float yaw, String actorName);
		virtual ~EntityBird();

		/** implement override functions from EntityLiving */
		virtual bool getCanSpawnHere();

		/** implement override functions from EntityLivingBase */
		virtual void onUpdate();
		virtual void onLivingUpdate();
		virtual bool canBeCollidedWith();
		virtual bool isFlyEntity() { return true; }

		/** Takes a coordinate in and returns a weight to determine how likely this creature will try to path to the block.	*/
		virtual float getBlockPathWeight(const BlockPos& pos) { return 0.0F; }

		/** Returns true if entity has a path to follow */
		bool hasPath() { return pathToEntity != NULL; }

		/** sets the Entities walk path in EntityBird */
		void setPathToEntity(PathEntity* path);

		/** Returns current entities target */
		Entity* getEntityToAttack() { return entityToAttack; }

		/** Sets the entity which is to be attacked. */
		void setTarget(Entity* pEntity) { entityToAttack = pEntity; }

		bool isWithinHomeDistanceCurrentPosition() { return isWithinHomeDistanceFromPosition(position.getFloor()); }

		bool isWithinHomeDistanceFromPosition(const BlockPos& pos);

		void setHomePosAndDistance(const BlockPos& pos, int dis) { homePosition = pos; maximumHomeDistance = (float)dis; }

		const BlockPos& getHomePosition() { return homePosition; }

		float getMaximumHomeDistance() { return maximumHomeDistance; }

		void detachHome() { maximumHomeDistance = -1.0F; }

		bool hasHome() { return maximumHomeDistance != -1.0F; }

		void getPathToEntity(Entity* endEntity);

		virtual void setDead();

		virtual float getAttackDistance();

		virtual PathEntity* getPathEntity() {
			return pathToEntity;
		}

		void decrTicksToFree() {
			if (isDead) {
				--m_delayTicksToFree;
			}
		}

		void setNeedRegMovePath(bool needReg) { this->m_bNeedRegMovePath = needReg; }
		void cleanRegMovePath() {
			m_movePath.clear();
		}

		std::vector<BlockPos>& getRegMovePath() { return m_movePath; }

	protected:
		/*** Disables a mob's ability to move on its own while true. */
		bool isMovementCeased() { return false; }

		virtual void moveFlying(float strafe, float forward, float speed);

		float limitAngle(float angle1, float angle2, float angleMax);



		/* for path finder test */
		void getPathToEntityPlayer();

		/** Basic mob attack. Default to touch of death in EntityBird. Overridden by each mob to define their attack. */
		virtual void attackEntity(Entity* pEntity, float damage) {}

		/** Finds the closest player within 16 blocks to attack, or null if this Entity isn't interested in attacking (Animals, Spiders at day, peaceful PigZombies). */
		virtual Entity* findPlayerToAttack() { return NULL; }

		/** implement override functions from EntityLivingBase */
		virtual void updateEntityActionState();

		virtual void onLeashDistance(float par1) {}

		virtual void updateFallState(float y, bool onGroundIn) {};

	};


}
#endif