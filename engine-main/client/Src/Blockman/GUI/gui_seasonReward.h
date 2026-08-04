#ifndef __GUI_SEASON_REWARD_HEADER__
#define __GUI_SEASON_REWARD_HEADER__

#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_seasonReward : public gui_layout
	{
	private:

	public:
		gui_seasonReward();
		~gui_seasonReward();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool onCloseClick(const EventArgs & events);
		bool onUpdateLastSeason(int honorId, int rank, int honor);
		bool onUpdateCurrentSeason(int honorId, int rank, int honor, int endTime);

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif