#pragma once
#include "Entity/EntityFishHook.h"
#include "World/World.h"

namespace BLOCKMAN
{
	class EntityFishHookClient : public EntityFishHook
	{
		RTTI_DECLARE(EntityFishHookClient);

	public:
		bool m_hasBeenLaunched = false;

	private:
		bool m_isShowCannonBtn = false;

	public:
		EntityFishHookClient(World* pWorld, EntityLivingBase* pAngler);

		void onUpdate() override;
	};
}