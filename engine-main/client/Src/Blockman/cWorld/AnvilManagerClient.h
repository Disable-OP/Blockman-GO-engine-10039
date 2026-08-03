#ifndef __ANVIL_MANAGER_CLIENT_HEADER__
#define __ANVIL_MANAGER_CLIENT_HEADER__

#include "cWorld/AnvilClient.h"

#include "WorldGenerator/AnvilManager.h"

#include "Core.h"

namespace BLOCKMAN
{
	class AnvilManagerClient : public AnvilManager
	{
		using AnvilManager::AnvilManager;
	private:
		std::shared_ptr<Anvil> createAnvil(int anvilX, int anvilZ) const override
		{
			return make_shared<AnvilClient>(anvilX, anvilZ);
		}
	};
}

#endif // !__ANVIL_MANAGER_CLIENT_HEADER__
