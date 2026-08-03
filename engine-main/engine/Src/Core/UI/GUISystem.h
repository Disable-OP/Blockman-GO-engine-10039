/********************************************************************
filename: 	GUISystem.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_SYSTEM_H__
#define __GUI_SYSTEM_H__

#include "Core.h"
#include "Object/Singleton.h"
#include "Render/RenderQueue.h"
#include "Render/RenderState.h"
#include "GUICommonDef.h"

struct lua_State;


#if _DEBUG
//#define TOUCHLOG 
#endif

namespace LORD
{
	class GUISystem;
	class GUIRenderManager;
	class GUIWindowManager;
	class GUIImagesetManager;
	class GUIFontManager;
	class GUIHyperlinkManager;
	class GUIAnimatMgr;
	class GUIWindow;
	class GUIEditBox;
	class ScriptModule;
	class GUIRenderImage;
	class GUIRenderTargetManager;
	class Touch;

	class  LORD_CORE_API GUIManualRender : public ManualRenderListener
	{
	public:
		virtual void render(bool uiRender = false);
		GUISystem* m_pGUISystem;

	};

	class LORD_CORE_API GUISystem : public ObjectAlloc, public Singleton<GUISystem>
	{
		friend class GUIEditBox;

	public:

		GUISystem();
		~GUISystem();

		static GUISystem* Instance();

		void	Initialize(uint nWidth, uint nHeight, lua_State* ls = NULL);

		void	Destroy();

		void	UpdateGUI();

		void	RenderGUI();

		void	OnSized(uint nWidth, uint nHeight);

		inline void	GetLogicSize(uint& nLogicWidth, uint& nLogicHeight) 
		{ 
			nLogicWidth = m_nLogicWidth; 
			nLogicHeight = m_nLogicHeight;
		}

		void	SetRootWindow(GUIWindow* pRoot);
		GUIWindow* GetRootWindow() { return m_pRootWindow; }

		void	NotifyWindowDestroy(GUIWindow* pWindow);

		GUIWindow* GetTargetGUIWindow(const Vector2& point) const;

// 		bool	TouchDown(size_t nIndex, const Vector2& position);
// 
// 		bool	TouchUp(size_t nIndex, const Vector2& position);
// 
// 		bool	TouchMove(size_t nIndex, const Vector2& position);

		// 新的事件处理
		void	handleTouchDown(const Touch* touch);
		void	handleTouchMove(Touch* touch);
		void	handleTouchUp(const Touch* touch);
		void	handleTouchCancel(const Touch* touch);

// 		// 手柄输入
// 		// 同级向前寻找focus
// 		void	MoveFocusForward();
// 		// 同级向后寻找focus
// 		void	MoveFocusBackward();
// 		// 进入下一级
// 		void	EnterFocus();
// 		// 返回上一级
// 		void	BackFocus();
// 
// 		// 按键消息
// 		void	FocusDown();
// 		void	FocusUp();
// 		void	SetFocusWindow(GUIWindow* pWindow) { m_pFocusWindow = pWindow; }

		void	SetDraggingWindow(GUIWindow* pWindow) { m_pDraggingWindow = pWindow; }

		GUIEditBox* GetActiveEditBox() { return m_pActiveEditBox; }

		void	SetActiveEditBox(GUIEditBox* pEditBox);

		ScriptModule*	GetScriptModule() { return m_pScriptModule;}

		void			SetScriptModule(ScriptModule* pScriptModule) { m_pScriptModule = pScriptModule; }

		// call back function
//		void	SetActiveEditBoxCallback(ACTIVE_EDITBOX_CALLBACK pFunc) { m_pActiveEditboxCallback = pFunc; }
//		ACTIVE_EDITBOX_CALLBACK GetActiveEditBoxCallback() { return m_pActiveEditboxCallback; }

		void	SetHyperlinkCallback(GUI_HYPERLINK_CALLBACK pFunc) { m_pHyperlinkCallback = pFunc; }
		GUI_HYPERLINK_CALLBACK GetHyperlinkCallback() { return m_pHyperlinkCallback; }

		void	SetRenderHitTextCallback(GUI_RENDER_HITTEXT_CALLBACK pFunc) { m_pRenderHitTextCallback = pFunc; }
		GUI_RENDER_HITTEXT_CALLBACK GetRenderHitTextCallback() { return m_pRenderHitTextCallback; }

		uint	GetScreenWidth() { return m_nScreenWidth; }
		uint	GetScreenHeight() { return m_nScreenHeight; }

		// screen to logic
		void	AdaptSize(uint nWidth, uint nHeight, uint& outWidth, uint& outHeight);

		// 第一个参数是真正的屏幕坐标
		void	AdaptPosition(const Vector2& position, Vector2& outPosition);

		// 第一个参数是相对于黑边边缘的屏幕坐标
		void	AdaptPositionEx(const Vector2& position, Vector2& outPosition);

		// world to screen
		Vector2 WorldPostionToScreen(const Vector3& worldpos);

		// 计算2d billboard的ui位置
		void	set2DBillBoardPosition(GUIWindow* window, const Vector3& worldPosition, const Real width, const Real height);

		// logic to screen
		void	LogicPositinToScreenPosition(const Vector2 position, Vector2& outPosition);

		void	LogicRectToScreenRect(const ERect& inRect, ERect& outRect);

		const GUIString& GetUnderLineImageName() { return m_strUnderLineImage; }

		const Color& GetHyperLinkClickColor() { return m_cHyperLinkClickColor; }

		const GUIString& GetFocusImage() { return m_strFocusImage; }

		const GUIString& GetScrollThumbImage() { return m_strScrollThumbImage; }

		void setEnableRender( bool _val )
		{
			m_enableRender = _val;
		}

		bool getEnableRender() const
		{
			return m_enableRender;
		}

		void setAdateSize(bool setting) { m_bAdaptSize = setting; }

		void setStandardCoordinate(const Vector2& size) { m_standardCoordinate = size; }

		float getHitTestOffset() { return m_hitTestOffset; }

		void setHitTestOffset(float offset) { m_hitTestOffset = offset; }

		void setCurrentLanguage(String lang) { m_currentLanguage = lang; }

	protected:
		bool m_enableRender;
	private:
		void	LoadGUIResource();

		GUIManualRender				m_GUIManualRender;
		GUIRenderManager*			m_pRenderManager = nullptr;
		GUIWindowManager*			m_pWindowManager = nullptr;
		GUIImagesetManager*			m_pImagesetManager = nullptr;
		GUIFontManager*				m_pFontManager = nullptr;
		GUIHyperlinkManager*		m_pHyperlinkManager = nullptr;
		GUIAnimatMgr *              m_pAnimateMgr = nullptr;
		ScriptModule*				m_pScriptModule = nullptr;
		GUIRenderTargetManager*		m_pRenderTargetManager = nullptr;
		GUIWindow*					m_pRootWindow = nullptr;
		GUIWindow*					m_pMotionWindow[MAX_MOTION_TARGET_COUNT];
		GUIWindow*					m_pModalWindow = nullptr;
		GUIEditBox*					m_pActiveEditBox = nullptr;
		//ACTIVE_EDITBOX_CALLBACK		m_pActiveEditboxCallback;
		GUI_HYPERLINK_CALLBACK		m_pHyperlinkCallback;
		GUI_RENDER_HITTEXT_CALLBACK m_pRenderHitTextCallback;
		uint						m_nScreenWidth;
		uint						m_nScreenHeight;
		uint						m_nLogicWidth;
		uint						m_nLogicHeight;
		GUIString					m_strUnderLineImage;
		Color						m_cHyperLinkClickColor;
		// 用来计算是否move
		Vector2						m_touchDownPosition;
		// 记录上一次的位置
		Vector2						m_lastPosition;
		// 长按的标志位，因为是移动的时候才判断，所以只触发一次
		bool						m_bLongTouchDownFlag;

		// 手柄控制的时候focus的图
		GUIString					m_strFocusImage;
		GUIWindow*					m_pFocusWindow = nullptr;
		GUIRenderImage*				m_pFocusRenderImage = nullptr;

		// 双击事件保存点击的窗口
		GUIWindow*					m_pClickWindow = nullptr;
		ui32						m_nClickTime;

		// scroll
		GUIWindow*					m_pScrollWindow = nullptr;
		// capture window
		GUIWindow*					m_pCaptureWindow = nullptr;
		// dragging window
		GUIWindow*					m_pDraggingWindow = nullptr;

		GUIString					m_strScrollThumbImage;

		ui8							m_RenderStartIndex;
		ui8							m_RenderEndIndex;

		// 是否适配分辨率
		bool						m_bAdaptSize;
		// 基准分辨率尺寸
		Vector2						m_standardCoordinate;

		float						m_hitTestOffset;

		//当前游戏的环境语言
		String                      m_currentLanguage;


#ifdef TOUCHLOG
		Log*	m_TouchLog = nullptr;
		Log* _createLog();
		void TouchMessage(const char* formats, ...);
#endif // TOUCHLOG

	};
}


#endif
