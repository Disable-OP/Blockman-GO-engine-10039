#pragma once
#include "Core.h"
namespace BLOCKMAN
{
	class EntityBirdAI;
	enum class BirdAIType:int
	{
		emFollow = 0,
		emGather = 1,
		emTranlate = 2,
		emAttack
	};

	template<typename AIType>
	class AIStatusBase
	{
	public:
		virtual AIType getType() = 0;
		virtual bool needEnter() = 0;
		virtual bool needLeave() = 0;
		virtual void enter(AIType  preType) = 0;
		virtual void leave() = 0;
		virtual void update() = 0;
	};


	template<BirdAIType TYPE>
	class BirdAIStatusBase:public AIStatusBase<BirdAIType>
	{
	public:
		BirdAIStatusBase(EntityBirdAI *bird) :m_birdAi(bird) {}

		virtual BirdAIType getType() { return TYPE; }
		virtual void enter(BirdAIType  preType) { m_running = true; };
		virtual void leave() { m_running = false; }
		virtual void update() {}
		virtual bool needEnter() { return !m_running; }
		virtual bool needLeave() { return false; }

		EntityBirdAI * m_birdAi;
		bool m_running = false;
	};

	class BirdAIStatusFollow :public BirdAIStatusBase<BirdAIType::emFollow>
	{
		enum FollowState
		{
			emFllowIdl = 0,
			emFllowYaw = 1,
			emFllowPos = 2,
			emFllowPosWhileLeaveMaster = 3
		};
	public:
		using super = BirdAIStatusBase<BirdAIType::emFollow>;
		using super::super;

		virtual void enter(BirdAIType preType);
		virtual void update();
		virtual bool needEnter();
		virtual bool needLeave();

	private:
		bool isMasterYawChange();
		bool isMasterPosChange();

		bool tryFollowPosWhileLeaveMaster();
		bool tryFollowPos();
		bool tryFollowYaw();
		bool tryFollowIdle();

		void updateMasterMoveInfo();

	private:
		LORD::ui32 m_lastEnterIdleTime = 0;

		float m_masterYaw[2];
		LORD::Vector3 m_masterPos[2];

		LORD::Vector3 m_lastForceFollowTo = LORD::Vector3::INVALID;
		LORD::Vector3 m_lastForcePatrolTo = LORD::Vector3::INVALID;
		FollowState m_state = emFllowIdl;
	};

	class BirdAIStatusGather :public BirdAIStatusBase<BirdAIType::emGather>
	{
	public:
		using super = BirdAIStatusBase<BirdAIType::emGather>;
		using super::super;

		virtual void enter(BirdAIType preType);
		virtual void leave();
		virtual void update();
		virtual bool needEnter();
		virtual bool needLeave();

	private:
		bool tryFindAndMoveToFruitPos();
	private:
		LORD::ui32 m_lastEnterGatherTime = 0;
		bool m_bInMoveToNewPos = false;
		bool m_bhasOver = false;
		LORD::Vector3 m_lastGotherPosition = LORD::Vector3::INVALID;

	};

	class BirdAIStatusTranslate :public BirdAIStatusBase<BirdAIType::emTranlate>
	{
		enum TranslateState
		{
			emStart = 0,
			emMoveToMaster = 1,
			emMoveToHome = 2,
			emInTranslate = 3,
			emMoveBack = 4,
			emOver
		};

	public:
		using super = BirdAIStatusBase<BirdAIType::emTranlate>;
		using super::super;

		virtual void enter(BirdAIType preType);
		virtual void leave();
		virtual void update();
		virtual bool needEnter();
		virtual bool needLeave();

	private:
		LORD::ui32  m_enterLastStateTime = 0;
		TranslateState m_state;
	};

	class BirdAIStatusAttack :public BirdAIStatusBase<BirdAIType::emAttack>
	{
	public:
		using super = BirdAIStatusBase<BirdAIType::emAttack>;
		using super::super;

		virtual void enter(BirdAIType preType);
		virtual void leave();
		virtual void update();
		virtual bool needEnter();
		virtual bool needLeave();

	private:
		bool moveToAttacker();

		bool m_bMoveBack = false;
		bool m_battackOver = false;

		int  m_currentEntityId = 0;
		LORD::ui64 m_lastAttackTime = 0;
	};


}