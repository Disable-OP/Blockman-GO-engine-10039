#include "stdafx.h"
#include "ShellInterfaceWin32.h"

namespace ClothesPlug {

	ShellInterfaceWin32::ShellInterfaceWin32()
	{

	}

	ShellInterfaceWin32::~ShellInterfaceWin32()
	{

	}

	void ShellInterfaceWin32::gameExit()
	{
		PostMessage((HWND)m_wndHandle, WM_CLOSE, 0, 0);
	}
};