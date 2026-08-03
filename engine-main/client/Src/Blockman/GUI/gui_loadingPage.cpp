#include <functional>
#include "gui_loadingPage.h"
#include "UI/GUIWindow.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUISystem.h"
#include "UI/GUIProgressBar.h"
#include "UI/GUIStaticText.h"
#include "UI/GUISceneMap.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "Object/Root.h"
#include "game.h"
#include "Util/ClientDataReport.h"

namespace BLOCKMAN
{
	gui_loadingPage::gui_loadingPage() :
		gui_layout("LoadingPage.json"),
		m_textLaodTipMessage(nullptr),
		m_buttonClose(nullptr),
		m_showType(ShowType::DOWNLOAD_MAP),
		m_progress(0.0),
		m_pregressFileSize(0.0)
	{
		
		m_subscriptionGuard.add(NetworkConnectionDisconnectEvent::subscribe(std::bind(&gui_loadingPage::onNetworkConnectionDisconnect, this)));
		m_subscriptionGuard.add(NetworkConnectionKickOutEvent::subscribe(std::bind(&gui_loadingPage::onNetworkConnectionKickOut, this)));
		m_subscriptionGuard.add(NetworkConnectionTimeoutEvent::subscribe(std::bind(&gui_loadingPage::onNetworkConnectionTimeout, this)));
		m_subscriptionGuard.add(BeKickedEvent::subscribe(std::bind(&gui_loadingPage::onBeKicked, this)));

		m_subscriptionGuard.add(NetworkConnectionSuccessfulEvent::subscribe(makeDataSetter(m_showType, ShowType::NETWORK_CONNECT_SUCCESSFUL)));
		m_subscriptionGuard.add(NetworkConnectionFailureEvent::subscribe(makeDataSetter(m_showType, ShowType::NETWORK_CONNECT_FAILURE)));
		m_subscriptionGuard.add(LoadWorldStartEvent::subscribe(makeDataSetter(m_showType, ShowType::LOAD_WORLD_START)));
		m_subscriptionGuard.add(LoadWorldEndEvent::subscribe(makeDataSetter(m_showType, ShowType::LOAD_WORLD_END)));
		m_subscriptionGuard.add(LoginFailureEvent::subscribe(makeDataSetter(m_showType, ShowType::LOGIN_FAILURE)));
		m_subscriptionGuard.add(LoginTokenErrorEvent::subscribe(makeDataSetter(m_showType, ShowType::LOGIN_TOKEN_ERROR)));
		m_subscriptionGuard.add(LoginGetUserAttrFailEvent::subscribe(makeDataSetter(m_showType, ShowType::LOGIN_GET_USER_ATTR_FAIL)));
		m_subscriptionGuard.add(LoginWaitRoleAttrFailEvent::subscribe(makeDataSetter(m_showType, ShowType::LOGIN_WAIT_ROLE_ATTR_FAIL)));
		m_subscriptionGuard.add(LoginResponseTimeoutEvent::subscribe(makeDataSetter(m_showType, ShowType::LOGIN_RESPONSE_TIMEOUT)));
		m_subscriptionGuard.add(CheckCSVersionSuccEvent::subscribe(makeDataSetter(m_showType, ShowType::CHECK_VERSION_SUCCESS)));
		m_subscriptionGuard.add(CheckCSVersionFailEvent::subscribe(makeDataSetter(m_showType, ShowType::CHECK_VERSION_FAILURE)));
		m_subscriptionGuard.add(LoginSuccEvent::subscribe(makeDataSetter(m_loginSucc, true)));

		m_subscriptionGuard.add(DownloadMapProgressEvent::subscribe(std::bind(&gui_loadingPage::onDownloadMapProgress, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(DownloadMapSuccessEvent::subscribe(std::bind(&gui_loadingPage::onDownloadMapSuccess, this)));
		m_subscriptionGuard.add(DownloadMapFailureEvent::subscribe(std::bind(&gui_loadingPage::onDownloadMapFailure, this)));
		m_subscriptionGuard.add(GameoverEvent::subscribe(std::bind(&gui_loadingPage::onGameover, this, std::placeholders::_1)));
	}

	gui_loadingPage::~gui_loadingPage()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_loadingPage::onLoad()
	{
		m_rootWindow->SetTouchable(false);
		GUIWindowManager* guiWindowMgr = GUIWindowManager::Instance();
		m_buttonClose = getWindow<GUIButton>("LoadingPage-Button-close");
		m_textLaodTipMessage = getWindow<GUIStaticText>("LoadingPage-TextLaodTipMessage");
		getWindow("LoadingPage-Loading-Failure")->SetVisible(false);
		getWindow("LoadingPage-Loading")->SetVisible(true);
		getWindow("LoadingPage-LoadingTip")->SetVisible(true);
		m_buttonClose->subscribeEvent(EventButtonClick, std::bind(&gui_loadingPage::btnCloseClick, this, std::placeholders::_1));
		m_buttonClose->SetText(getString(LanguageKey::GUI_EXIT_GAME));
	
		m_pProgressBar = getWindow<GUIProgressBar>("LoadingPage-LoadingProgress");
		m_pProgressPercent = getWindow<GUIStaticText>("LoadingPage-LoadingTip-Percent");
		onDownloadMapProgress(0.0f);
		playSoundByType(SoundType::ST_Loading);
	}

	void gui_loadingPage::onUpdate(ui32 nTimeElapse)
	{
		refreshUi();
	}

	void gui_loadingPage::refreshUi()
	{
		if (!m_rootWindow->IsVisible())
			return;

		if (GameClient::CGame::Instance()->isEnterBack())
			return;

		switch (m_showType)
		{
		case ShowType::DOWNLOAD_MAP:
			RefreshProgress(m_progress, m_pregressFileSize);
			m_textLaodTipMessage->SetText(getString(LanguageKey::GUI_LOADING_PAGE_DOWNLOAD_MAP_LOADING));
			break;
		case ShowType::DOWNLOAD_MAP_SUCCESS:
			m_textLaodTipMessage->SetVisible(true);
			m_textLaodTipMessage->SetText(getString(LanguageKey::GUI_LOADING_PAGE_CONNECTED_SERVER_CONNECTING));
			setLoadingSucUi();
			break;
		case ShowType::NETWORK_CONNECT_SUCCESSFUL:
		case ShowType::LOAD_WORLD_START:
			//connected successfully
			m_textLaodTipMessage->SetVisible(true);
			m_textLaodTipMessage->SetText(getString(LanguageKey::GUI_CHECKING_VERSION));
			setLoadingSucUi();
			break;

		case ShowType::CHECK_VERSION_SUCCESS:
			//check version successfully
			m_textLaodTipMessage->SetVisible(true);
			m_textLaodTipMessage->SetText(getString(LanguageKey::GUI_LOADING_PAGE_PLAYER_LOGIN_LOGGINGIN));
			setLoadingSucUi();
			break;

		case  ShowType::LOAD_WORLD_END:
			if (!m_loginSucc) {
				break;
			}

			//load done
			m_textLaodTipMessage->SetVisible(true);
			setLoadingSucUi();
			m_textLaodTipMessage->SetText("load completed...");
			hide();
			if (!m_initMainGui) {
				BLOCKMAN::Blockman::Instance()->onGameReady();
				m_initMainGui = true;
			}
			stopOrCloseBackgroundMusic();
			break;

		case ShowType::NETWORK_CONNECT_FAILURE:
			//connected failure
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_LOADING_PAGE_CONNECTED_SERVER_FAILED));
			break;

		case ShowType::LOGIN_FAILURE:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_LOADING_PAGE_PLAYER_LOGIN_FAILURE));
			break;
		case ShowType::LOGIN_TOKEN_ERROR:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_LOADING_PAGE_PLAYER_LOGIN_TOKEN_ERROR));
			break;
		case ShowType::LOGIN_GET_USER_ATTR_FAIL:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_LOADING_PAGE_PLAYER_LOGIN_GET_USER_ATTR_FAIL));
			break;
		case ShowType::LOGIN_WAIT_ROLE_ATTR_FAIL:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_LOADING_PAGE_PLAYER_LOGIN_WAIT_ROLE_ATTR_FAIL));
			break;
		case ShowType::LOGIN_RESPONSE_TIMEOUT:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_LOADING_PAGE_PLAYER_LOGIN_RESPONSE_TIMEOUT));
			break;

		case ShowType::CHECK_VERSION_FAILURE:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_CHECK_VERSION_FAILURE));
			break;

		case ShowType::DOWNLOAD_MAP_FAILURE:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_LOADING_PAGE_DOWNLOAD_MAP_FAILURE));
			break;

		case ShowType::GAMEOVER:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(m_overMessage));
			break;
		case ShowType::NETWORK_DISCONNECT:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_MESSAGE_NETWORK_CONNECTION_DISCONNECT));
			break;
		case ShowType::NETWORK_KICK_OUT:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_MESSAGE_NETWORK_CONNECTION_KICK_OUT));
			break;
		case ShowType::BE_KICKED:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(getString(LanguageKey::GUI_MESSAGE_ACCOUNT_BE_KICKED));
			break;
		case ShowType::NETWORK_TIMEOUT:
			setLoadingFailUi();
			getWindow("LoadingPage-Msg-Failure")->SetText(m_overMessage.c_str());
			break;
		}

		if (m_oldShowType!=m_showType)
		{
			m_oldShowType = m_showType;
			switch (m_showType) {
			case ShowType::NETWORK_CONNECT_FAILURE:
			case ShowType::LOGIN_FAILURE:
			case ShowType::LOGIN_TOKEN_ERROR:
			case ShowType::LOGIN_GET_USER_ATTR_FAIL:
			case ShowType::LOGIN_WAIT_ROLE_ATTR_FAIL:
			case ShowType::LOGIN_RESPONSE_TIMEOUT:
			case ShowType::CHECK_VERSION_FAILURE:
			case ShowType::DOWNLOAD_MAP_FAILURE:
			case ShowType::GAMEOVER:
			case ShowType::NETWORK_DISCONNECT:
			case ShowType::NETWORK_KICK_OUT:
			case ShowType::NETWORK_TIMEOUT:
				stopOrCloseBackgroundMusic();
				LordLogInfo("gui_loadingPage_stopOrCloseBackgroundMusic %d", (int)m_showType);
				break;
			}
		}
	}

	bool gui_loadingPage::onDownloadMapProgress(float progess, float fileSize)
	{
		this->m_progress = progess;
		this->m_pregressFileSize = fileSize;
		m_showType = ShowType::DOWNLOAD_MAP;
		refreshUi();
		return true;
	}

	bool gui_loadingPage::onDownloadMapSuccess()
	{
		m_showType = ShowType::DOWNLOAD_MAP_SUCCESS;
		refreshUi();
		return true;
	}

	bool gui_loadingPage::onDownloadMapFailure()
	{
		m_showType = ShowType::DOWNLOAD_MAP_FAILURE;
		refreshUi();
		return true;
	}

	bool gui_loadingPage::onBeKicked()
	{
		m_showType = ShowType::BE_KICKED;
		if (!m_rootWindow->IsVisible()) {
			getParent()->showLoadPage();
		}
		refreshUi();
		return true;
	}

	bool gui_loadingPage::onNetworkConnectionDisconnect()
	{
		if (m_showType == ShowType::BE_KICKED || m_showType == ShowType::GAMEOVER)
		{
			// just remind the user be kicked or game over
			return true;
		}
		m_showType = ShowType::NETWORK_DISCONNECT;
		if (!m_rootWindow->IsVisible()) 
		{
			getParent()->showLoadPage();
		}
		LordLogInfo("gui_loadingPage_onNetworkConnectionDisconnect");
		return true;
	}

	bool gui_loadingPage::onNetworkConnectionKickOut()
	{
		m_showType = ShowType::NETWORK_KICK_OUT;
		if (!m_rootWindow->IsVisible()) 
		{
			getParent()->showLoadPage();
		}
		LordLogInfo("gui_loadingPage_onNetworkConnectionKickOut");
		return false;
	}

	bool gui_loadingPage::onNetworkConnectionTimeout()
	{
		if (m_showType != ShowType::GAMEOVER || m_overMessage.length() == 0)
		{
			m_showType = ShowType::NETWORK_TIMEOUT;
			m_overMessage = getString(LanguageKey::GUI_MESSAGE_NETWORK_CONNECTION_NETWORK_ERROR).c_str();
		}
		
		if (!m_rootWindow->IsVisible()) {
			getParent()->showLoadPage();
		}
		LordLogInfo("gui_loadingPage_onNetworkConnectionTimeout");
		return false;
	}

	bool gui_loadingPage::onGameover(String overMessage)
	{
		if (m_showType == ShowType::GAMEOVER)
			return true;
		m_showType = ShowType::GAMEOVER;
		if (!m_rootWindow->IsVisible()) {
			getParent()->showLoadPage();
		}
		m_overMessage = overMessage;
		LordLogInfo("gui_loadingPage_onGameover");
		SoundSystem::Instance()->stopBackgroundMusic(false);
		return true;
	}

	bool gui_loadingPage::btnCloseClick(const EventArgs)
	{
		GameClient::CGame::Instance()->exitGame();
		return true;
	}

	void gui_loadingPage::RefreshProgress(float fProgress, float fileSize)
	{
		m_pProgressBar->SetProgress(fProgress);

		int percent = (int)(fProgress * 100);
		float fileSizeMb = fileSize / 1024 / 1024;
		LORD::String text = StringUtil::Format(getString(LanguageKey::GUI_GAME_LOADING).c_str(), percent, fileSizeMb);
		m_pProgressPercent->SetText(text.c_str());
	}

	void gui_loadingPage::setLoadingSucUi()
	{
		getWindow("LoadingPage-Loading")->SetVisible(true);
		getWindow("LoadingPage-LoadingTip")->SetVisible(false);
		getWindow("LoadingPage-Loading-Failure")->SetVisible(false);
	}

	void gui_loadingPage::setLoadingFailUi()
	{
		getWindow("LoadingPage-Loading")->SetVisible(false);
		getWindow("LoadingPage-LoadingTip")->SetVisible(false);
		getWindow("LoadingPage-Loading-Failure")->SetVisible(true);
	}

	void gui_loadingPage::SetLoadingProgress(float fProgress)
	{
		RefreshProgress(fProgress);
	}

}
