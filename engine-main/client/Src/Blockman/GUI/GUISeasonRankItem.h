#ifndef __GUI_SEASON_RANK_ITEM_HEADER__
#define __GUI_SEASON_RANK_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SeasonSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISeasonRankItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUISeasonRankItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISeasonRankItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetSeasonRankItem(int seasonId, SeasonRankItem rank);

	};
}

#endif