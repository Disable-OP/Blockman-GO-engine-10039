#pragma once

#include "Entity/EntityCreature.h"
#include "Behaviac/behaviac_generated/types/behaviac_types.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	class EntityCreatureAI : public EntityCreature
	{
		RTTI_DECLARE(EntityCreatureAI);

	public:
		EntityCreatureAI(World* pWorld, int monsterId, Vector3& pos, float yaw, String actorName);
		virtual ~EntityCreatureAI();

		/** implement override functions from EntityCreature */
		virtual void onUpdate();
		virtual void onLivingUpdate();

		bool    attackEntityFrom(DamageSource* pSource, float amount);
		void	selectBtTreeByMonsterId(int monsterId);
		void	setCurrentAI(const char* pBtTree);
		void	setAIActive(bool bActive);
		std::string	  getLoadBTTree() { return  m_sLoadBTTree; }
		int     getCreatureAIType() { return m_monsterType; }
		bool    changeCreatureAction(CreatureActionState nActionStatus);
		void	onAttackPlayer(EntityPlayer* player);
		void    attackPerformance(EntityLivingBase* targeter);
		void    groupAttackPerformance();
		void    sendSkillEffect(int effectId);

		void unloadAgent();
	private:
		bool loadAgent();
		void updateAgent();

		BaseAgent * m_pAgentPlayer	= NULL;
		std::string		  m_sLoadBTTree		= "";
		bool			  m_bIsAIActive		= false;
		int				  m_monsterType		= 0;
		std::mutex		  m_loadAgentMutex;
		int				  m_btExecCnt = 0;
	};
}