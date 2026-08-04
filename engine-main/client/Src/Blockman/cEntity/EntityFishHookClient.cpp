#include "EntityFishHookClient.h"
 #include "EntityPlayerSPMP.h"

namespace BLOCKMAN
{
	BLOCKMAN::EntityFishHookClient::EntityFishHookClient(World * pWorld, EntityLivingBase * pAngler)
		: EntityFishHook(pWorld, pAngler)
	{
	}

	void BLOCKMAN::EntityFishHookClient::onUpdate()
	{
		EntityFishHook::onUpdate();

		if (m_targetEntityId)
		{
			EntityPlayerSPMP* myPlayer = dynamic_cast<EntityPlayerSPMP*>(getTarget());
			if (myPlayer)
			{
				position.y -= 1.62f;
			}
		}
	}
}