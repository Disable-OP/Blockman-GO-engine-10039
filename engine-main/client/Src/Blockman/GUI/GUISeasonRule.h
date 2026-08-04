#ifndef __GUI_SEASON_RULE_HEADER__
#define __GUI_SEASON_RULE_HEADER__

#include "gui_layout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISeasonRule : public gui_layout
	{
	private:

	public:
		GUISeasonRule();
		~GUISeasonRule();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;

	private:
		bool onContentClick(const EventArgs & events);
		bool onUpdateSeasonRule(const String & data);
	};
}

#endif