#pragma once
#include "Entity/EntityBird.h"
#include "Behaviac/behaviac_generated/types/behaviac_types.h"
#include "Object/Root.h"
#include "Blockman/Entity/BirdAIStatus.h"

class TableVisitor;
namespace BLOCKMAN
{

	class EntityBirdAI : public  EntityBird
	{
		RTTI_DECLARE(EntityBirdAI);

		static constexpr char * emAttack = "attack";      //攻击力
		static constexpr char * emAttackCD = "attackCD";   //  攻击CD
		static constexpr char * emFlyHigh = "flyHigh";       //飞行高度
		static constexpr char * emConvertCD = "convertCD";   // 转换速度
		static constexpr char * emCollectCD = "collectCD";     //采集CD
		static constexpr char * emFlySpeed = "flySpeed";     // 飞行速度

		static constexpr int attackRange = 12;
		static constexpr LORD::ui32 longTimeUpdateGroups = 20;
		
	public:
		static constexpr LORD::ui32 getLongTimeUpdateGroups() 
		{
			return longTimeUpdateGroups;
		}

		struct BlockPosCompare
		{
			bool operator()(const LORD::Vector3i & lhs, const LORD::Vector3i& rhs) const
			{
				if (lhs.x < rhs.x)
				{
					return true;
				}
				if (lhs.x > rhs.x)
				{
					return false;
				}
				if (lhs.y < rhs.y)
				{
					return true;
				}
				if (lhs.y > rhs.y)
				{
					return false;
				}
				if (lhs.z < rhs.z)
				{
					return true;
				}
				return false;
			}
		};


	public:
		EntityBirdAI(World* pWorld, std::shared_ptr<TableVisitor> birdMgr, Vector3i homePos,
			i64 birdId, Vector3& pos, float yaw, String actorName);
		virtual ~EntityBirdAI();

		/** implement override functions from EntityBird */
		virtual void onUpdate();
		virtual void onLivingUpdate();

		bool changeBirdAction(BirdActionState nActionStatus);
		void attackPerformance(int id);

		Vector3 findPotralPosNearMaster();
		void patrolReport();
		void tryMoveTo(const Vector3& pos);
		bool tryTeleportTo(const Vector3 &pos);
		bool isMoveOver() { return this->getPathEntity() == nullptr && this->getForceMoveTo().isInvalid(); }
		bool hasLeaveMaster();

		void initAIStatus();
		void updateAIStatus();

		bool isCanCollect();
		Vector3 findValidGatherPosition(const Vector3 &curGotherPos);
		std::vector<LORD::Vector3i>& collectGatherPosition();
		bool isValidGatherPosition(const Vector3i  &pos);
		void freeGatherPos(const Vector3i & pos);
		void regGatherPos(const Vector3i& pos);
		bool isGatherPosUsed(const Vector3i & pos);

		bool hasReciveTranslateCMD();
		void resetReciveTranslateCMDFlag();
		bool hasReciveInteruptTranslateCMD();
		void interruptTranslate();
		void beginTranslate();
		void finishTranslate();
		Vector3 getBirdHousePosition();

		Vector3 findTranslateNearMasterPos();

		int findMonster(int curAttacker);
		Vector3 getMonsterPosition(int id);

		Vector3i getRalitivePathToMaster();
		
		float getAttackCD();
		float getFlyHigh();
		virtual float getFlySpeedAddition();
		float getConvertCD();
		float getCollectCD();
		virtual float getFlySpeed() { return 0.2f + getFlySpeedAddition(); }
		virtual void forceMoveTo(const Vector3 &toPos);

		LORD::ui32 getLongTimeUpdateIndex() { return m_longTimeUpdateIndex; }
	private:
		float  getFloatAttrFromLua(const char * attr);
		int    getBelongSameMasterBirdNum();
		int    getBirdAreaLength();
		void dispathLongTimeUpdateGroup();
	private:

		std::shared_ptr<TableVisitor> m_birdManagerInScript;
		std::vector<std::shared_ptr<AIStatusBase<BirdAIType>>> m_allAIStatus;
		int m_curAIIndex = -1;

		int m_lastBirdNum = 0;
		int m_currentBirdArealen = 0;

		int m_lastFindMonsterTime = 0;

		float m_lastGetSpeedAddition = FLT_MAX;
		float m_lastGetCollectCD = FLT_MAX;
		float m_lastGetConvertCD = FLT_MAX;
		float m_lastGetAttackCD = FLT_MAX;
		
		Vector3i m_lastPotrolRelativePos = Vector3i::INVALID;
		
		LORD::ui32 m_longTimeUpdateIndex = 0;

		static std::set<LORD::Vector3i, BlockPosCompare> m_usedGotherPos;
		static std::map<LORD::ui64, std::vector<LORD::Vector3i>> m_validGatherPosSetForPlayer;
		static LORD::ui32 m_longTimeOPUpdateGroup[longTimeUpdateGroups];
	};
}