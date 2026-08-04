#ifndef __ANVIL_MANAGER_SERVER_HEADER__
#define __ANVIL_MANAGER_SERVER_HEADER__

#include "Core.h"
#include "WorldGenerator/AnvilManager.h"
#include "Blockman/World/AnvilServer.h"

namespace BLOCKMAN
{
	class AnvilManagerServer : public AnvilManager
	{
		using AnvilManager::AnvilManager;
	private:
		std::shared_ptr<Anvil> createAnvil(int anvilX, int anvilZ) const override
		{
			return make_shared<AnvilServer>(anvilX, anvilZ);
		}
	};
}

#endif // !__ANVIL_MANAGER_CLIENT_HEADER__
