#include "MciPlayer.h"
#include "stdlib.h"
#include <time.h>

#define WIN_CLASS_NAME        "CocosDenshionCallbackWnd"
#define BREAK_IF(cond)      if (cond) break;

namespace Lord {

static HINSTANCE s_hInstance;
static MCIERROR  s_mciError;

static LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
float getRandFloat(float min, float max);

float getRandFloat(float min, float max)
{
	srand((unsigned int)(time(NULL)));                                                                    
	return min + rand() / (float)RAND_MAX * (max - min);
}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (!hwnd)
		return;

	MciPlayer * pPlayer = (MciPlayer *)GetWindowLong(hwnd, GWL_USERDATA);
	if (!pPlayer || !pPlayer->m_hDev || idEvent != pPlayer->m_nSoundID)
		return;

	mciSendCommand(pPlayer->m_hDev, MCI_SEEK, MCI_SEEK_TO_START, 0);

	MCI_PLAY_PARMS mciPlay = { 0 };
	mciPlay.dwCallback = (DWORD)hwnd;
	mciSendCommand(pPlayer->m_hDev, MCI_PLAY, MCI_NOTIFY, (DWORD)&mciPlay);
}

MciPlayer::MciPlayer()
: m_hWnd(NULL)
, m_hDev(NULL)
, m_nSoundID(0)
, m_uTimes(0)
, m_fMinInterval(0.0f)
, m_fMaxInterval(0.0f)
, m_bLoop(false)
, m_bPlaying(false)
{
    if (! s_hInstance)
    {
        s_hInstance = GetModuleHandle( NULL );            // Grab An Instance For Our Window

        WNDCLASS  wc;        // Windows Class Structure

        // Redraw On Size, And Own DC For Window.
        wc.style          = 0;  
        wc.lpfnWndProc    = _SoundPlayProc;                    // WndProc Handles Messages
        wc.cbClsExtra     = 0;                              // No Extra Window Data
        wc.cbWndExtra     = 0;                                // No Extra Window Data
        wc.hInstance      = s_hInstance;                    // Set The Instance
        wc.hIcon          = 0;                                // Load The Default Icon
        wc.hCursor        = LoadCursor( NULL, IDC_ARROW );    // Load The Arrow Pointer
        wc.hbrBackground  = NULL;                           // No Background Required For GL
        wc.lpszMenuName   = NULL;                           // We Don't Want A Menu
        wc.lpszClassName  = WIN_CLASS_NAME;                 // Set The Class Name

        if (! RegisterClass(&wc)
            && 1410 != GetLastError())
        {
            return;
        }
    }

    m_hWnd = CreateWindowEx(
        WS_EX_APPWINDOW,                                    // Extended Style For The Window
        WIN_CLASS_NAME,                                        // Class Name
        NULL,                                        // Window Title
        WS_POPUPWINDOW,/*WS_OVERLAPPEDWINDOW*/               // Defined Window Style
        0, 0,                                                // Window Position
        0,                                                    // Window Width
        0,                                                    // Window Height
        NULL,                                                // No Parent Window
        NULL,                                                // No Menu
        s_hInstance,                                        // Instance
        NULL );
    if (m_hWnd)
    {
        SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
    }
}

MciPlayer::~MciPlayer()
{
    Close();
    DestroyWindow(m_hWnd);
}

void MciPlayer::Open(const char* pFileName, UINT uId)
{

//     WCHAR * pBuf = NULL;
    do 
    {
        BREAK_IF(! pFileName || ! m_hWnd);
        int nLen = (int)strlen(pFileName);
        BREAK_IF(! nLen);
//         pBuf = new WCHAR[nLen + 1];
//         BREAK_IF(! pBuf);
//         MultiByteToWideChar(CP_ACP, 0, pFileName, nLen + 1, pBuf, nLen + 1);

        Close();

        MCI_OPEN_PARMS mciOpen = {0};
        MCIERROR mciError;
        mciOpen.lpstrDeviceType = (LPCTSTR)MCI_ALL_DEVICE_ID;
        mciOpen.lpstrElementName = pFileName;

        mciError = mciSendCommand(0,MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)&mciOpen);
        BREAK_IF(mciError);

        m_hDev = mciOpen.wDeviceID;
        m_nSoundID = uId;
        m_bPlaying = false;
    } while (0);
}

void MciPlayer::Play(bool bLoop, float fMinInterval, float fMaxInterval)
{
    if (! m_hDev)
    {
        return;
    }
    MCI_PLAY_PARMS mciPlay = {0};
    mciPlay.dwCallback = (DWORD_PTR)m_hWnd;
    s_mciError = mciSendCommand(m_hDev,MCI_PLAY, MCI_FROM|MCI_NOTIFY,(DWORD)&mciPlay);
    if (! s_mciError)
    {
        m_bPlaying = true;
        m_uTimes = 0;
		m_bLoop = bLoop;
		m_fMinInterval = fMinInterval;
		m_fMaxInterval = fMaxInterval;
    }
}

void MciPlayer::Close()
{
    if (m_bPlaying)
    {
        Stop();
    }
    if (m_hDev)
    {
         _SendGenericCommand(MCI_CLOSE);
    }
    m_hDev = 0;
    m_bPlaying = false;
}

void MciPlayer::Pause()
{
    _SendGenericCommand(MCI_PAUSE);
}

void MciPlayer::Resume()
{
    _SendGenericCommand(MCI_RESUME);
}

void MciPlayer::Stop()
{
    _SendGenericCommand(MCI_STOP);
    m_bPlaying = false;
}

void MciPlayer::Rewind()
{
    if (! m_hDev)
    {
        return;
    }
    mciSendCommand(m_hDev, MCI_SEEK, MCI_SEEK_TO_START, 0);

    MCI_PLAY_PARMS mciPlay = {0};
    mciPlay.dwCallback = (DWORD)m_hWnd;
    m_bPlaying = mciSendCommand(m_hDev, MCI_PLAY, MCI_NOTIFY,(DWORD)&mciPlay) ? false : true;
}

bool MciPlayer::IsPlaying()
{
    return m_bPlaying;
}

UINT MciPlayer::GetSoundID()
{
    return m_nSoundID;
}

//////////////////////////////////////////////////////////////////////////
// private member
//////////////////////////////////////////////////////////////////////////

void MciPlayer::_SendGenericCommand(int nCommand)
{
    if (! m_hDev)
    {
        return;
    }
    mciSendCommand(m_hDev, nCommand, 0, 0);
}

//////////////////////////////////////////////////////////////////////////
// static function
//////////////////////////////////////////////////////////////////////////

LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    MciPlayer * pPlayer = NULL;
    if (MM_MCINOTIFY == Msg 
        && MCI_NOTIFY_SUCCESSFUL == wParam
        &&(pPlayer = (MciPlayer *)GetWindowLong(hWnd, GWL_USERDATA)))
    {
        if (pPlayer->m_bLoop)
        {
			float interval = getRandFloat(pPlayer->m_fMinInterval, pPlayer->m_fMaxInterval);
			if (interval < 0.5f)
			{
				mciSendCommand(lParam, MCI_SEEK, MCI_SEEK_TO_START, 0);

				MCI_PLAY_PARMS mciPlay = { 0 };
				mciPlay.dwCallback = (DWORD)hWnd;
				mciSendCommand(lParam, MCI_PLAY, MCI_NOTIFY, (DWORD)&mciPlay);
			}
			else
			{
				SetTimer(hWnd, pPlayer->m_nSoundID, (int)(interval * 1000), TimerProc);
			}
        }
        else
        {
            pPlayer->m_bPlaying = false;
        }
        return 0;
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

} // end of namespace CocosDenshion
