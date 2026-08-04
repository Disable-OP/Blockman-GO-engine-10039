#pragma once
#ifndef __GUI_RANCH_COMMUNITY_HEADER__
#define __GUI_RANCH_COMMUNITY_HEADER__

#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/gui_ranch.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIStaticText.h"
#include "GUI/GuiRanchFriendOperate.h"
#include "GUI/GuiRanchOrderOperate.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_ranchCommunity : public  gui_layout
	{
	private:
		enum class ViewId {
			FRIEDN,
			CLAN,
			MAIL
		};

	public:
		gui_ranchCommunity();
		virtual ~gui_ranchCommunity();
		void onLoad() override;
		void onHide() override;
		void onUpdate(ui32 nTimeElapse) override;
		gui_ranch* getParent()
		{
			return dynamic_cast<gui_ranch*>(m_parentLayout);
		}
		bool onRadioChange(const EventArgs & events, ViewId viewId);

	private:
		bool mailChange();
		bool friendChange();
		bool clanMemberChange();
		
		bool onClick(const EventArgs& args);
		bool hideHelpOperate(const EventArgs& args);
		bool onOrederItemClick(const UVector2& pos, const RanchOrderInfo& info);
		
		bool hideFriendOperate();
		bool onFriendClick(i64 userId, const String & viewName);

		bool onHelpsChange(i64 userId);
		bool onFriendUpdate();

	private:
		void changeOffset(ViewId viewId, int size);

	private:
		SubscriptionGuard m_subscriptionGuard;

		GUIStaticText* m_tvGiftValue = nullptr;
		GUIListBox* m_helpsList = nullptr;
		GUILayout* m_helpsLayout = nullptr;
		GuiRanchFriendOperate* m_friendOperate = nullptr;
		GuiRanchOrderOperate* m_orderOperate = nullptr;
		map<ViewId, GUIString>::type m_radioTextMap;
		map<ViewId, GUIListHorizontalBox*>::type m_radioPanelMap;
		map<ViewId, GUIRadioButton*>::type m_radioMap;
		map<ViewId, float>::type m_lvOffsetMap;

	};
}

#endif