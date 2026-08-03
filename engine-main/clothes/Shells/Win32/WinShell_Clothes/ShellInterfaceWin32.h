#pragma once

#ifndef _SHELL_INTERFACE_WIN32_H_
#define _SHELL_INTERFACE_WIN32_H_

#include "stdafx.h"
#include "ShellInterface.h"

namespace ClothesPlug {

	class ShellInterfaceWin32 : public ShellInterface
	{
	public:
		ShellInterfaceWin32();
		virtual ~ShellInterfaceWin32();
		virtual void   gameExit();

		void setWndHandle(unsigned int handle) { m_wndHandle = handle; }
	private:
		
		unsigned int m_wndHandle;
	};
}

#endif