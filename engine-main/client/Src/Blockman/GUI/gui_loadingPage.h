#pragma once
#ifndef __GUI_LOADING_PAGE_HEADER__
#define __GUI_LOADING_PAGE_HEADER__
#include "Core.h"
#include "gui_layout.h"
#include "UI/GUIWindow.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include <atomic>

using namespace LORD;

namespace BLOCKMAN
{
	class gui_loadingPage : public gui_layout 
	{
	
	public:
		enum class ShowType
		{
			DOWNLOAD_MAP,
			NETWORK_CONNECT_SUCCESSFUL,
			NETWORK_CONNECT_FAILURE,
			NETWORK_DISCONNECT,
			NETWORK_KICK_OUT,
			NETWORK_TIMEOUT,
			LOAD_WORLD_START,
			LOAD_WORLD_END,
			LOGIN_FAILURE,
			LOGIN_TOKEN_ERROR,
			LOGIN_GET_USER_ATTR_FAIL,
			LOGIN_WAIT_ROLE_ATTR_FAIL,
			LOGIN_RESPONSE_TIMEOUT,
			DOWNLOAD_MAP_SUCCESS,
			DOWNLOAD_MAP_FAILURE,
			CHECK_VERSION_SUCCESS,
			CHECK_VERSION_FAILURE,
			BE_KICKED,
			GAMEOVER
		};
	private:
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	public:
		gui_loadingPage();
		virtual ~gui_loadingPage();
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		void refreshUi();
		bool onDownloadMapProgress(float progess, float fileSize = 0);
		bool onDownloadMapSuccess();
		bool onDownloadMapFailure();
		bool onNetworkConnectionDisconnect();
		bool onBeKicked();
		bool onNetworkConnectionKickOut();
		bool onNetworkConnectionTimeout();
		bool onGameover(String overMessage);
		bool btnCloseClick(const EventArgs);
		void setShowType(ShowType showType) { this->m_showType = showType; }
		void SetLoadingProgress(float fProgress);
		void RefreshProgress(float fProgress, float fileSize = 0);
		void setLoadingSucUi();
		void setLoadingFailUi();

	private:
		GUIStaticText* m_textLaodTipMessage;
		GUIButton* m_buttonClose;
		ShowType m_showType;
		ShowType m_oldShowType = ShowType::DOWNLOAD_MAP;
		float m_progress = 0.0;
		float m_pregressFileSize = 0.0;
		String m_overMessage = "";
		GUIProgressBar* m_pProgressBar = NULL;
		GUIStaticText*	m_pProgressPercent = NULL;

		bool m_loginSucc = false;
		bool m_initMainGui = false;
        
        SubscriptionGuard m_subscriptionGuard;
	};

}
#endif
