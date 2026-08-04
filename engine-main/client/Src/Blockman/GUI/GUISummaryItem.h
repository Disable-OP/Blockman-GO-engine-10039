#pragma once
#include "UI/GUIWindow.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIStaticText.h"
#include "BattleSummary.h"
#include "gui_rank.h"

using namespace LORD;
using namespace GameClient;

namespace BLOCKMAN
{
	class GuiSummaryItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUIWindow* m_window = nullptr;
		GUIStaticImage* m_rankBgImage = nullptr;
		GUIStaticImage* m_rankImage = nullptr;
		GUIWindow* m_doubleRank = nullptr;
		GUIStaticImage* m_doubleRankTen = nullptr;
		GUIStaticImage* m_doubleRankOne = nullptr;
		GUIStaticImage* m_result = nullptr;

		GUIStaticText* m_name = nullptr;
		GUIStaticText* m_reward = nullptr;
		GUIStaticImage* m_rewardIcon = nullptr;
		GUIStaticImage* m_vipIcon = nullptr;
		GUIStaticImage* m_awardMaxIcon = nullptr;
		GUIStaticText* m_score = nullptr;

		GUIButton* m_btn = nullptr;
		GUIButton* m_visitBtn = nullptr;
		GUIStaticText* m_buildWarScore = nullptr;

		ResultEntry::ResultEntryPtr	m_resultEntry;

		bool onNameClick(const EventArgs & events);
		void removeComponents();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiSummaryItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiSummaryItem() = default;

		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;
		void RefreshItem(ResultEntry::ResultEntryPtr result);
		void RefreshItem(RankData* rankData);
		void setButtonVisible(bool isVisible);
		void setButtonText(GUIString str);
		bool onClick(const EventArgs&);
		bool onClickVisit(const EventArgs&);

		static bool GetImageByRank(int rank, String & bgImage, String & image, String & tenImage, Color & textColor, Color & borderColor);
		static bool GetNumberImage(int num, String & image);

	private:
		ui64 m_userId = 0;
	};
}
