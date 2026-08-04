/********************************************************************
filename: 	Win32InputBox.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#include "Core.h"
#if (LORD_PLATFORM == LORD_PLATFORM_WINDOWS)
#ifndef __WIN32INPUTBOX__
#define __WIN32INPUTBOX__

#include <windows.h>
#include <tchar.h>
#include <string>

class CWin32InputBox;

// Structure used to orient the inputbox behavior
struct WIN32INPUTBOX_PARAM
{
	friend class CWin32InputBox;

	//
	IN OPTIONAL bool bMultiline;

	// Pass this as none zero so to use this memory dlg template
	IN OPTIONAL LPVOID DlgTemplateData;

	// Pass this as none ZERO so to load DLGTEMPLATE from resources
	IN OPTIONAL LPCSTR DlgTemplateName;

	// passing both "DlgTemplateName" and "DlgTemplateData" ZERO will cause
	// the dialog to use his default embedded resource

	// Center on monitor or owner window?
	IN OPTIONAL bool bCenter;

	// Want to add more styles to the dialog?
	IN OPTIONAL DWORD dwStylesPlus, dwStylesMinus;
	IN OPTIONAL DWORD dwExStylesPlus, dwExStylesMinus;

	IN LPCSTR szTitle, szPrompt;

	// Return buffer
	OUT LPSTR szResult;
	IN DWORD nResultSize;

	// Owner window
	HWND hwndOwner;
	HINSTANCE hInstance;

	short xPos, yPos;

	WIN32INPUTBOX_PARAM();
private:
	HWND hDlg;
};

class CWin32InputBox
{
private:
	WIN32INPUTBOX_PARAM *_param;
	static LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
	HWND _hwndEditCtrl;

	void InitDialog();
	void SetParam(WIN32INPUTBOX_PARAM *);
	WIN32INPUTBOX_PARAM * GetParam();

public:

	CWin32InputBox(WIN32INPUTBOX_PARAM *);
	~CWin32InputBox();

	static INT_PTR InputBoxEx(WIN32INPUTBOX_PARAM *);
	static INT_PTR InputBox(
		LPCSTR szTitle,
		LPCSTR szPrompt,
		LPSTR szResult,
		DWORD nResultSize,
		bool bMultiLine = false,
		HWND hwndParent = 0);

	static std::string AnsiToUtf8(std::string strAnsi);
	static std::string Utf8ToAnsi(std::string strUTF8);
};

#endif
#endif
