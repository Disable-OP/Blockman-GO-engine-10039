#include <iostream>
#include "game.h"
#include "gui_gameTipDialog.h"
#include "Util/ClientEvents.h"
#include "GUI/GuiSlotTable.h"
#include "UI/GUIString.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "Util/ClientDataReport.h"
#include "Util/LanguageManager.h"
#include "Blockman/cWorld/Blockman.h"
#include "Blockman/cEntity/EntityPlayerSPMP.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_gameTipDialog::gui_gameTipDialog() :
		gui_layout("GameTipDialog.json"),
		m_btnSure(nullptr),
		m_btnCancel(nullptr),
		m_msgText(nullptr),
		m_showType(ShowType::CLOSE_GAME)
	{
	}
	gui_gameTipDialog::~gui_gameTipDialog()
	{
//        LordSafeDelete(m_btnSure);
//        LordSafeDelete(m_btnCancel);
//        LordSafeDelete(m_msgText);
	}

	void gui_gameTipDialog::onLoad()
	{
		m_rootWindow->SetTouchable(true);
		GUIWindowManager* guiWindowMgr = GUIWindowManager::Instance();
		m_btnSure = getWindow<GUIButton>("GameTipDialog-BtnSure");
		m_btnCancel = getWindow<GUIButton>("GameTipDialog-BtnCancel");
		m_msgText = getWindow<GUIStaticText>("GameTipDialog-MsgText");
		m_btnSure->subscribeEvent(EventButtonClick, std::bind(&gui_gameTipDialog::btnSureClick, this, std::placeholders::_1));
		m_btnCancel->subscribeEvent(EventButtonClick, std::bind(&gui_gameTipDialog::btnCancelClick, this, std::placeholders::_1));
	}

	void gui_gameTipDialog::onUpdate(ui32 nTimeElapse)
	{
		refreshUi();
	}

	void gui_gameTipDialog::refreshUi()
	{
		if (!isShown()) return;

		String sureText = LanguageManager::Instance()->getString(LanguageKey::GUI_SURE);
		String cancelText = LanguageManager::Instance()->getString(LanguageKey::GUI_CANCEL);
		String msgText = "";
		switch (m_showType)
		{
		case ShowType::DEATH:
			sureText = LanguageManager::Instance()->getString(LanguageKey::GUI_REBIRTH);
			cancelText = LanguageManager::Instance()->getString(LanguageKey::GUI_CLOSE);
			msgText = LanguageManager::Instance()->getString(LanguageKey::GUI_MESSAGE_DEATH);
			getWindow<GUIStaticImage>("GameTipDialog-Bg")->SetDrawColor(Color(241.0f / 255, 69.0f / 255, 27.0f / 255, 0.4f));
			//m_btnSure->SetVisible(false);
			break;
		case ShowType::CLOSE_GAME:
			sureText = LanguageManager::Instance()->getString(LanguageKey::GUI_SURE);
			cancelText = LanguageManager::Instance()->getString(LanguageKey::GUI_CANCEL);
			msgText = LanguageManager::Instance()->getString(LanguageKey::GUI_MESSAGE_EXIT_GAME);
			getWindow<GUIStaticImage>("GameTipDialog-Bg")->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 0.4f));
			break;
		case ShowType::NETWORK_DISCONNECT:
			sureText = LanguageManager::Instance()->getString(LanguageKey::GUI_SURE);
			cancelText = LanguageManager::Instance()->getString(LanguageKey::GUI_CANCEL);
			msgText = LanguageManager::Instance()->getString(LanguageKey::GUI_MESSAGE_NETWORK_CONNECTION_DISCONNECT);
			getWindow<GUIStaticImage>("GameTipDialog-Bg")->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 0.4f));
			break;
		}

		m_btnSure->SetText(sureText.c_str());
		m_btnCancel->SetText(cancelText.c_str());
		m_msgText->SetText(msgText.c_str());
	}

	void gui_gameTipDialog::setShowType(ShowType showType)
	{
		this->m_showType = showType;
	}

	gui_gameTipDialog::ShowType gui_gameTipDialog::getShowType()
	{
		return m_showType;
	}


	bool gui_gameTipDialog::btnSureClick(const EventArgs)
	{
		switch (m_showType)
		{
		case ShowType::DEATH:
			//重生逻辑
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendRebirth();
			getParent()->showGameTipDialogManage(false);
			break;
		case ShowType::CLOSE_GAME:
		case ShowType::NETWORK_DISCONNECT:
			//关闭游戏
			GameClient::CGame::Instance()->exitGame();
			break;
		}
		return true;
	}

	bool gui_gameTipDialog::btnCancelClick(const EventArgs)
	{

		switch (m_showType)
		{
		case ShowType::DEATH:
		case ShowType::NETWORK_DISCONNECT:
			getParent()->showGameTipDialogManage(false);
			GameClient::CGame::Instance()->exitGame(); // just for test, need to recover

			break;
		case ShowType::CLOSE_GAME:
			//关闭窗口
			getParent()->showGameTipDialogManage(false);
			break;
		}
		return true;
	}

}
