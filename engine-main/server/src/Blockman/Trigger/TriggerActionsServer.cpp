#include "TriggerActionsServer.h"
#include "Server.h"

using namespace LORD;

namespace BLOCKMAN
{
	void TriggerActionExplode::doAction(const BlockPos& position, const TriggerActivatorList & activators) const
	{
		EntityLivingBase* activator = nullptr;
		if (activators.size() > 0 && activators[0].isEntity())
		{
			activator = dynamic_cast<EntityLivingBase*>(activators[0].toEntity());
		}
		Server::Instance()->getWorld()->createExplosion(nullptr, activator, Vector3(position) + 0.5f, m_power, true);
	}
}