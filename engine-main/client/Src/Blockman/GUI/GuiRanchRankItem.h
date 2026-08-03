#pragma once
#ifndef __GUI_RANCH_RANK_ITEM_HEADER__
#define __GUI_RANCH_RANK_ITEM_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "World/Ranch.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiRanchRankItem : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		RanchRank m_ranchRank;

	private:
		void removeComponents();

	public:
		GuiRanchRankItem(const GUIWindowType& nType, const GUIString& strName);
		~GuiRanchRankItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;
		void setRank(const RanchRank& rank);
		void setRankType(i32 type) { m_rankType = type; }
	private:
		bool onClick(const EventArgs & args);
		void changeRankValue(i32 rank);

	private:
		i32 m_rankType = 1;

		GUIStaticText * m_tvRankValue = nullptr;
		GUIStaticImage* m_ivRankIcon = nullptr;
		GUIStaticText * m_tvNickName = nullptr;
		GUIButton * m_tvUserLevel = nullptr;
		GUIStaticText * m_tvClanUserNum = nullptr;
		GUIStaticImage* m_ivIcon = nullptr;
		GUIStaticText * m_tvValue = nullptr;
		GUIButton * m_btnVisit = nullptr;



	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
