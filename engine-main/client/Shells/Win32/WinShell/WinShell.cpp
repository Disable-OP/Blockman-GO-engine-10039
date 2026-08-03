// WinShell.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WinShell.h"
#include "Game.h"
#include "UI/Touch.h"
#include "Core.h"
#include "Object/Root.h"
#include "Win32SoundEngine.h"
#include "Audio/SoundSystem.h"
#include "ShellInterfaceWin32.h"

#include <dbghelp.h>   

#define MAX_LOADSTRING 100

//#define MULTITHREA_RENDER

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

float fAspectRaido = 2.0f / 3.0f;

int g_nWindowWidth = 854;
int g_nWindowHeight = 480;// int(800 * fAspectRaido + 10);

unsigned int g_hWnd = 0;
GameClient::CGame* g_pGame = NULL;
LORD::SoundEngine* g_pSoundEngine = NULL;
GameClient::ShellInterfaceWin32* shellInterface = NULL;

bool g_bMousePressed = false;
bool g_bMultiTouch = false;

HINSTANCE g_hInstance = NULL;
float g_TouchDownPositionX = 0;
float g_TouchDownPositionY = 0;

struct EVENTINFO
{
	EVENTINFO(UINT msg, WPARAM wP, LPARAM lP)
		: message(msg)
		, wParam(wP)
		, lParam(lP)
	{}

	UINT message;
	WPARAM wParam;
	LPARAM lParam;
};
typedef std::vector<EVENTINFO> EVENTLIST;
EVENTLIST g_EventList_first;
EVENTLIST g_EventList_second;
EVENTLIST g_EventList_thread;
EVENTLIST* g_CurEventList = &g_EventList_first;
EVENTLIST* g_SwapEventList = &g_EventList_second;
bool g_SwapEvent = false;
CRITICAL_SECTION g_CriticalSection;

using namespace LORD;

bool g_bDestroy = false;
HANDLE mainThread = 0;
HANDLE hThread = 0;
HANDLE hUpdateThread = 0;

static unsigned __stdcall UpdateThreadCallBackProc(void *pvArg)
{
	g_pGame->UpdateFiles();
	hUpdateThread = 0;
	return 0;
}

#ifdef MULTITHREA_RENDER

static unsigned __stdcall ThreadCallBackProc(void *pvArg)
{
	// 初始化client
	g_pGame = LordNew GameClient::CGame;
	g_pGame->InitConfig("./");
	// 	g_pGame->CheckVersion("./");
	// 	g_pGame->UpdateFiles("./");
	if (g_pGame->CheckVersion("./") == 2)
	{
		hUpdateThread = (HANDLE)_beginthreadex(
			NULL,			        	// SD
			0,				        	// initial stack size
			UpdateThreadCallBackProc,			// thread function
			NULL, 				// thread argument
			0,				        	// creation option
			NULL						// thread identifier
			);
		if (!hUpdateThread)
		{
			MessageBox(0, "_beginthreadex fail! Thread::Init", "Error", 0);
			return false;
		}
	}

	while (hUpdateThread)
	{
		Sleep(10);
	}

	g_pSoundEngine = LordNew LORD::Win32SoundEngine;
	g_pGame->InitGame("./", "./", g_nWindowWidth, g_nWindowHeight, g_hWnd, g_pSoundEngine);
	while (!g_bDestroy)
	{
		g_pGame->MainTick();

		EnterCriticalSection(&g_CriticalSection);
		if (g_SwapEvent)
		{
			g_EventList_thread.assign(g_SwapEventList->begin(), g_SwapEventList->end());
			g_SwapEvent = false;
		}
		LeaveCriticalSection(&g_CriticalSection);

		for (EVENTLIST::iterator it = g_EventList_thread.begin(); it != g_EventList_thread.end(); ++it)
		{
			switch (it->message)
			{
			case WM_SIZE:
			{
				int nWidth = LOWORD(it->lParam);
				int nHeight = HIWORD(it->lParam);

				if (g_pGame)
				{
					g_pGame->OnSize(nWidth, nHeight);
				}
			}
			break;
			case WM_KEYDOWN:
			{
				if (it->wParam == VK_F1)
				{
					g_pSoundEngine->pauseBackgroundMusic();

				}
				if (it->wParam == VK_F2)
				{
					g_pSoundEngine->resumeBackgroundMusic();

				}
			}
			break;
			case WM_LBUTTONDOWN:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame)
				{
					//g_pGame->OnTouchDown(0, nPositionX, nPositionY);


					if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000))
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesBegin(num, ids, x, y);

						g_bMultiTouch = true;
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesBegin(num, ids, x, y);

						g_bMultiTouch = false;
					}

					g_TouchDownPositionX = nPositionX;
					g_TouchDownPositionY = nPositionY;
					g_bMousePressed = true;
				}
			}
			break;

			case WM_LBUTTONUP:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame && g_bMousePressed)
				{
					//g_pGame->OnTouchUp(0, nPositionX, nPositionY);

					if (g_bMultiTouch)
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesEnd(num, ids, x, y);
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesEnd(num, ids, x, y);
					}

					g_bMultiTouch = false;
					g_bMousePressed = false;
				}
			}
			break;

			case WM_MOUSEMOVE:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame && g_bMousePressed)
				{
					if (g_bMultiTouch)
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesMove(num, ids, x, y);
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesMove(num, ids, x, y);
					}
				}
			}
			break;
			case WM_MOUSELEAVE:
			{
				float nPositionX = (float)LOWORD(it->lParam);
				float nPositionY = (float)HIWORD(it->lParam);

				if (g_pGame && g_bMousePressed)
				{
					if (g_bMultiTouch)
					{
						int num = 2;
						intptr_t ids[] = { 0, 1 };

						float centerX = (float)g_nWindowWidth / 2;
						float centerY = (float)g_nWindowHeight / 2;

						float nPositionX2 = centerX + centerX - nPositionX;
						float nPositionY2 = centerY + centerY - nPositionY;

						float x[] = { nPositionX, nPositionX2 };
						float y[] = { nPositionY, nPositionY2 };

						g_pGame->handleTouchesCancel(num, ids, x, y);
					}
					else
					{
						int num = 1;
						intptr_t ids[] = { 0 };
						float x[] = { nPositionX };
						float y[] = { nPositionY };

						g_pGame->handleTouchesCancel(num, ids, x, y);
					}

					g_bMousePressed = false;
				}
			}
			break;
			}
		}
		g_EventList_thread.clear();
	}

	g_pGame->DestroyGame();

	g_pSoundEngine->end();
	LordSafeDelete(g_pSoundEngine);

	LordDelete g_pGame;
	g_pGame = NULL;

	SetEvent(mainThread);

	return 0;
}
#endif // MULTITHREA_RENDER

VOID GenerateDump(PEXCEPTION_POINTERS pException) throw()
{
	// 崩溃的时候打印lua堆栈
	if (g_pGame)
	{
		g_pGame->printLuaStack();
	}

	LORD::String dumpPath = "CrashDump";
	CreateDirectory(dumpPath.c_str(), NULL);
	LORD::String curTime = LORD::StringUtil::Format("%s %s", __DATE__, __TIME__);
	curTime = LORD::StringUtil::Replace(curTime, ':', '_');
	LORD::String strDumpFile = dumpPath + "\\" + curTime + ".dmp";
	HANDLE   hFile = CreateFile(strDumpFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION   ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pException;
		ExInfo.ClientPointers = NULL;

		//   write   the   dump
		BOOL   bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		CloseHandle(hFile);
	}
}

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void constructgame()
{
	//	// 初始化client
	g_pGame = new GameClient::CGame;

	// test对应serverlist的标注了test的服务器，在列表里只能看到test的服务器
	g_pGame->setPlatformInfo("test");

	if (!PathUtil::IsDirExist("./document"))
	{
		PathUtil::CreateDir("./document");
	}


	g_pSoundEngine = LordNew LORD::Win32SoundEngine;
	shellInterface = LordNew GameClient::ShellInterfaceWin32;
	shellInterface->setWndHandle(g_hWnd);
}

void destructgame()
{
	g_pGame->DestroyGame();
	Sleep(200);
	g_pSoundEngine->end();
	LordSafeDelete(g_pSoundEngine);

	LordSafeDelete(shellInterface);
	g_pGame->setShellInterface(NULL);

	delete g_pGame;
	g_pGame = NULL;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

#ifndef _DEBUG
	__try
	{
#endif
		g_hInstance = hInstance;

		// 初始化全局字符串
		LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadString(hInstance, IDC_WINSHELL, szWindowClass, MAX_LOADSTRING);
		MyRegisterClass(hInstance);

		// 执行应用程序初始化:
		if (!InitInstance(hInstance, nCmdShow))
		{
			return FALSE;
		}

		// 	MessageBox((HWND)g_hWnd, "请使用移动端连接外网服务器", "警告", 0);
		// 	return 0;

		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINSHELL));

#ifdef MULTITHREA_RENDER
		mainThread = CreateEvent(NULL, true, 0, NULL);

		InitializeCriticalSection(&g_CriticalSection);

		hThread = (HANDLE)_beginthreadex(
			NULL,			        	// SD
			0,				        	// initial stack size
			ThreadCallBackProc,			// thread function
			NULL, 				// thread argument
			0,				        	// creation option
			NULL						// thread identifier
			);
		if (!hThread)
		{
			MessageBox(0, "_beginthreadex fail! Thread::Init", "Error", 0);
			return false;
		}
#else
		constructgame();

		g_pGame->InitConfig("./");
		/*g_pGame->CheckVersion("./");
		g_pGame->UpdateFiles("./");*/
		if (g_pGame->CheckVersion("./") == 2)
		{
			hUpdateThread = (HANDLE)_beginthreadex(
				NULL,			        	// SD
				0,				        	// initial stack size
				UpdateThreadCallBackProc,			// thread function
				NULL, 				// thread argument
				0,				        	// creation option
				NULL						// thread identifier
				);
			if (!hUpdateThread)
			{
				MessageBox(0, "_beginthreadex fail! Thread::Init", "Error", 0);
				return false;
			}
		}

		while (hUpdateThread)
		{
			Sleep(10);
		}

		g_pGame->setShellInterface(shellInterface);
		g_pGame->InitGame("./", "./document/", g_nWindowWidth, g_nWindowHeight, g_hWnd, g_pSoundEngine);

#endif //MULTITHREA_RENDER
		while (true)
		{
			// Handle the windows messages.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// If windows signals to end the application then exit out.
#ifdef MULTITHREA_RENDER
			if (msg.message == WM_QUIT)
			{
				g_bDestroy = true;  // 先挂起游戏线程，再标记销毁
				WaitForSingleObject(mainThread, INFINITE); //挂起主线程，等待游戏线程执行销毁结束

				//销毁游戏线程，并销毁主线程信号
				WaitForSingleObject(hThread, INFINITE);
				CloseHandle(hThread);
				CloseHandle(mainThread);
				hThread = 0;
				mainThread = 0;
				break;
			}

			EnterCriticalSection(&g_CriticalSection);
			if (!g_SwapEvent)
			{
				g_SwapEventList->assign(g_CurEventList->begin(), g_CurEventList->end());
				g_CurEventList->clear();
				g_SwapEvent = true;
			}
			LeaveCriticalSection(&g_CriticalSection);

			Sleep(10);
		}
		DeleteCriticalSection(&g_CriticalSection);
#else
			if (msg.message == WM_QUIT)
			{
				break;
			}
			// 主tick 更新
			g_pGame->MainTick();
		}
		destructgame();
#endif

#ifndef _DEBUG
}
	__except (GenerateDump(GetExceptionInformation()), TerminateProcess(GetCurrentProcess(), 0), EXCEPTION_EXECUTE_HANDLER)
	{
	}
#endif

	return (int)msg.wParam;
}


void  relaunch()
{
	TCHAR moduleName[MAX_PATH];
	ZeroMemory(moduleName, sizeof(moduleName));
	GetModuleFileName(NULL, moduleName, MAX_PATH);

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	if (CreateProcess(NULL, moduleName, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
	{
		ExitProcess(0);
	}
}

//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINSHELL));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	int nPositionX = 0;
	int nPositionY = 0;

	// Place the window in the middle of the screen.
	nPositionX = (GetSystemMetrics(SM_CXSCREEN) - g_nWindowWidth) / 2;
	nPositionY = (GetSystemMetrics(SM_CYSCREEN) - g_nWindowHeight) / 2;

	RECT loRt;
	loRt.left = 0;
	loRt.top = 0;
	loRt.right = g_nWindowWidth;
	loRt.bottom = g_nWindowHeight;
	AdjustWindowRect(&loRt, WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_POPUP, FALSE);

	// Create the window with the screen settings and get the handle to it.
	hWnd = CreateWindowEx(WS_EX_APPWINDOW, szWindowClass, "WinShell-Blockman",
		WS_OVERLAPPEDWINDOW,
		nPositionX, nPositionY, loRt.right - loRt.left, loRt.bottom - loRt.top,
		NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = (unsigned int)hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
	{
#ifdef MULTITHREA_RENDER
		EVENTINFO info(message, wParam, lParam);
		g_CurEventList->push_back(info);
#else
		int nWidth = LOWORD(lParam);
		int nHeight = HIWORD(lParam);

		if (g_pGame)
		{
			g_pGame->OnSize(nWidth, nHeight);
		}
#endif
	}
	break;
	case WM_KEYDOWN:
	{
		{
#ifdef MULTITHREA_RENDER
			EVENTINFO info(message, wParam, lParam);
			g_CurEventList->push_back(info);
#else
			if (wParam == VK_F1)
			{
				g_pSoundEngine->pauseBackgroundMusic();

			}
			if (wParam == VK_F2)
			{
				g_pSoundEngine->resumeBackgroundMusic();

			}
#endif
		}
		g_pGame->handleKeyDown(wParam);
	}
	break;
	case WM_KEYUP:
	{
		g_pGame->handleKeyUp(wParam);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (g_pGame)
		{
			float nPositionX = (float)LOWORD(lParam);
			float nPositionY = (float)HIWORD(lParam);

			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesBegin(num, ids, x, y);
		}
	}
	break;

	case WM_LBUTTONUP:
	{
		float nPositionX = (float)LOWORD(lParam);
		float nPositionY = (float)HIWORD(lParam);

		if (g_pGame)
		{
			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesEnd(num, ids, x, y);
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		float nPositionX = (float)LOWORD(lParam);
		float nPositionY = (float)HIWORD(lParam);
		bool hoveUI = false;

		if (g_pGame)
		{
			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesMove(num, ids, x, y);

		}
		TRACKMOUSEEVENT lpET;
		lpET.cbSize = sizeof(TRACKMOUSEEVENT);
		lpET.dwFlags = TME_LEAVE;
		lpET.dwHoverTime = NULL;
		lpET.hwndTrack = (HWND)g_hWnd;
		TrackMouseEvent(&lpET);
	}
	break;
	case WM_MOUSELEAVE:
	{
		float nPositionX = (float)LOWORD(lParam);
		float nPositionY = (float)HIWORD(lParam);

		if (g_pGame)
		{
			int num = 1;
			intptr_t ids[] = { 0 };
			float x[] = { nPositionX };
			float y[] = { nPositionY };

			g_pGame->handleTouchesCancel(num, ids, x, y);
		}
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
