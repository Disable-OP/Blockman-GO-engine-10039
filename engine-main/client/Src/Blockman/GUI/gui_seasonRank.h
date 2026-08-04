#ifndef __GUI_SEASON_RANK_HEADER__
#define __GUI_SEASON_RANK_HEADER__

#include "gui_layout.h"
#include "GUI/GUISeasonRule.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_seasonRank : public gui_layout
	{
	private:
		GUISeasonRule* m_season_rule = nullptr;
	public:
		gui_seasonRank();
		~gui_seasonRank();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void onShow() override;

	private:
		bool onCloseClick(const EventArgs & events);
		bool onQuestionClick(const EventArgs & events);
		bool onArmoryClick(const EventArgs & events);
		bool onBattleClick(const EventArgs & events);
		bool onSeasonInit(int honorId, int rank, int honor, int endTime);
		bool onUpdateSeasonRankData(int type, const String & data);

	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif