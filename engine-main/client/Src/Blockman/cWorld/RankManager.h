#ifndef __RANK_MANAGER_HEADER__
#define __RANK_MANAGER_HEADER__
#include "Core.h"

using namespace LORD;

namespace GameClient
{
	class RankManager : public Singleton<RankManager>, public ObjectAlloc
	{
	public:
		RankManager();
		~RankManager();
	public:

		void setRankInfo(int entityId, String info);
		String getRankInfo(int entityId);

	private:

		std::map<int, String> m_RankInfoMap;

	};
}
#endif
