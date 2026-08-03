#include "stdafx.h"
#include "ShellInterfaceWin32.h"

namespace GameClient {

	GameClient::ShellInterfaceWin32::ShellInterfaceWin32()
	{

	}

	GameClient::ShellInterfaceWin32::~ShellInterfaceWin32()
	{

	}

	void GameClient::ShellInterfaceWin32::gameExit()
	{
		PostMessage((HWND)m_wndHandle, WM_CLOSE, 0, 0);
	}
};