#ifndef __GUI_SEASON_RANK_PANEL_HEADER__
#define __GUI_SEASON_RANK_PANEL_HEADER__

#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIListBox.h"
#include "UI/GUILayout.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SeasonSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISeasonRankPanel : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;
		SeasonHonorId m_honor_id = BRONZE;
		SeasonRankType m_type = TYPE_NONE;

		GUIListBox* m_rank_list = nullptr;
		GUIStaticText* m_reward_text = nullptr;
		GUILayout* m_reward_info = nullptr;
		GUIStaticText* m_honor_text = nullptr;
		GUIStaticText* m_honor_value = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();
		void initView();

	public:
		GUISeasonRankPanel(const GUIWindowType& nType, const GUIString& strName);
		~GUISeasonRankPanel() = default;

		void Destroy() override;

		void InitUI(SeasonHonorId honorId);
		void SetSeasonHonorId(SeasonHonorId honorId);
		void SetSeasonRankType(SeasonRankType type);
		void SetSeasonRankData(list<SeasonRankItem>::type ranks);
		void ShowSeasonRankData();
	};
}

#endif