#ifndef _MCI_PLAYER_WIN32_H_
#define _MCI_PLAYER_WIN32_H_

#include <Windows.h>
#include <mmsystem.h>
#include <Digitalv.h>

namespace Lord {

class MciPlayer
{
public:
    MciPlayer();
    ~MciPlayer();

    void Close();

    /**
    @brief 播放声音文件
    @param pFileName 播放的声音文件名称，需要包含文件的路径
    @param nTimes    播放声音文件的循环次数，默认值为 1，即播放一次
    */
    void Open(const char* pFileName, UINT uId);

    void Play(bool bLoop, float fMinInterval = 0.0f, float fMaxInterval = 0.0f);

    /**
    @brief 暂停播放声音
    */
    void Pause();

    /**
    @brief 继续播放声音
    */
    void Resume();

    /**
    @brief 停止播放声音
    */
    void Stop();

    /**
    @brief 重新播放
    */
    void Rewind();

    /**
    @brief 获取播放器当前是否正在播放中
    */
    bool IsPlaying();

    /**
    @brief 获取当前播放的音效 ID
    @return 当前播放的音效ID
    */
    UINT GetSoundID();

public:
	void Volume(UINT volume)
	{
		if (!m_hDev)
			return;
		MCI_DGV_SETAUDIO_PARMS mciParams = { 0 };
		mciParams.dwItem = MCI_DGV_SETAUDIO_VOLUME;
		mciParams.dwValue = volume;
		mciSendCommand(m_hDev, MCI_SETAUDIO, MCI_DGV_SETAUDIO_ITEM | MCI_DGV_SETAUDIO_VALUE, (DWORD)&mciParams);
	}

	UINT Volume() const
	{
		if (!m_hDev)
			return 0;
		MCI_STATUS_PARMS mciParams = { 0 };
		mciParams.dwItem = MCI_DGV_STATUS_VOLUME;
		mciSendCommand(m_hDev, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciParams);
		return mciParams.dwReturn;
	}

private:
    friend LRESULT WINAPI _SoundPlayProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	friend VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

    void _SendGenericCommand(int nCommand);

    HWND        m_hWnd;
    MCIDEVICEID m_hDev;
    UINT        m_nSoundID;
    UINT        m_uTimes;
    bool        m_bPlaying;
	bool		m_bLoop;
	float		m_fMinInterval; // 如果循环播放的话，最小播放间隔是多少
	float		m_fMaxInterval; 
};

} // end of namespace CocosDenshion

#endif
