#pragma once
#ifndef __GUI_PLAYER_LIST_HEADER__
#define __GUI_PLAYER_LIST_HEADER__

#include "Core.h"
#include "UI/GUIWindow.h"
#include "UI/GUIListBox.h"
#include "GuiPlayerItem.h"
#include "GUI/gui_layout.h"
#include "Util/SubscriptionGuard.h"
#include "UI/GUIStaticText.h"

using namespace LORD;

namespace BLOCKMAN {

	class gui_playerList : public gui_layout
	{
	public:

		enum class OperationType {
			NONE,
			NO_FRIEND,
			IS_FRIEND,
			AGREE_ADD_FRIEND,
			REQUEST_ADD_FRIEND,

			//error code
			AGREE_ADD_FRIEND_FAILURE = 10000,
			REQUEST_ADD_FRIEND_FAILURE
		};

	public:
		gui_playerList();
		~gui_playerList();

		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onUserIn(const  ui64 platformUserId, const  String& userName, const  String& teamName, const int teamId, const int curMunber, const int totalMunber);
		bool onUserOut(const ui64 playerPlatformId);
		bool onFriendOperationForServer(int operationType, ui64 playerPlatformId);
		bool onFriendOperationForAppHttpResult(int operationType, ui64 playerPlatformId);

	private:
		int getIndex(String viewName);
	private:
		map<ui64, GuiPlayerItem*>::type m_playerItemMap;
		map<ui64, GuiPlayerItem*>::type m_initBeforeMap;
		SubscriptionGuard m_subscriptionGuard;
		GUIListBox* m_playerList = nullptr;
		GUIStaticText* m_tipMessageView = nullptr;
		ui32 m_tipShopTime = 0; 
		String m_tipMessage;
	};
}
#endif
