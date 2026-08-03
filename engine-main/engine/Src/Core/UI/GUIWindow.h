/********************************************************************
filename: 	GUIWindow.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __GUI_WINDOW_H__
#define __GUI_WINDOW_H__

#include "Core.h"
#include "GUICommonDef.h"
#include "GUIUDim.h"
#include "GUIString.h"
#include "GUIEventSet.h"
#include "GUIAnimateController.h"
#include "GUIFlyAnimateController.h"
#include "GUIAction.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "rapidjson/document.h"

using namespace rapidxml;

namespace LORD
{
	class GUIFont;
	class GUIAnimateController;
	class Touch;
	class GUIImageset;

	class LORD_CORE_API GUIWindow: public ObjectAlloc, public EventSet
	{
	public:
		static bool isEditorMode;
		GUIWindow(GUIWindowType nType, const GUIString& strName);
		virtual ~GUIWindow();

		GUIWindowType GetType() { return m_nType;}
		const GUIString& GetName() { return m_strName; }

		void	Rename(const GUIString& strNewName);

		GUIMaterialType getMaterial() const { return m_material; }
		void	setMaterial(GUIMaterialType material) { m_material = material; }

		const GUIString& getMaskTextureName() const { return m_maskTextureName; }
		void setMaskTextureName(const GUIString& textureName);

		Texture* getMaskTexture() const { return m_maskTexture; }

// 		void	SetRenderTarget(bool bSetting);
// 		bool	IsRenderTarget() { return m_bIsRenderTarget; }

//		void	GetRenderCtxOffset(Vector2& offset);
//		void	InvalidateRT();

		void    SetEnabled(bool bSetting);
		bool	IsEnabled() { return m_bEnabled; }

// 		void    SetFlyAnimCount(int bSetting);
// 		int     getFlyAnimCount(){return m_FlyAnimCount; }
// 		bool	IsFlyAnimEnabled() { return (m_FlyAnimCount > 0); }
// 		void    SetSelectFlyAnim(int index);
// 		int     GetSelectFlyAnim();

		void	SetDrawColor(const Color& color);
		const Color& GetDrawColor() { return m_drawColor; }

		void	SetVisible(bool bSetting);
		bool	IsVisible() { return m_bVisible; }

		void	SetActive(bool bSetting);
		bool	IsActive() { return m_bActive; }

		void	SetTouchable(bool bSetting);
		bool	IsTouchable() { return m_bIsTouchable;}

		void	SetAutoSize(bool bSetting) { m_bAutoSize = bSetting; }
		bool	IsAutoSize() { return m_bAutoSize; }

		void	SetLevel(int nLevel);
		int		GetLevel() { return m_nLevel; }

		void	SetAlwaysOnTop(bool bSetting);
		bool	IsAlwaysOnTop() { return m_bAlwaysOnTop;}

		void	SetBackgroundColor(const Color& color);
		const Color& GetBackgroundColor() const { return m_backgroundColor; }

		void	SetAlpha(float fAlpha);
		float	GetAlpha() const { return m_fAlpha; }

		void SetMultiClicked(bool bSetting) { m_bMultiClicked = bSetting; }
		bool IsMultiClicked() { return m_bMultiClicked; }

		void SetTouchSlide( bool bSetting ) { m_bTouchSlide = bSetting; }
		bool IsTouchSlide() { return m_bTouchSlide; }

		void	setEnableDrag(bool enabled) { m_enableDrag = enabled; }
		bool	isEnableDrag() const { return m_enableDrag; }

		void	setDragging(bool dragging) { m_dragging = dragging; }
		bool	isDragging() const { return m_dragging; }

// 		void	SetAnimateType(GUIAnimateController::AnimateType nType);
// 		void	StartAnimate();
// 		void    StarFLyAnim();
// 		void    PlayFLyAnim(const String& name);

		//GUIAnimateController* GetAnimateController() { return m_pAnimateController; }

		void	SetClipChild(bool bClippedChild) { m_bIsClipChild = bClippedChild; }
		bool	IsClipChild() const { return m_bIsClipChild; }

		void	SetArea(const UDim& xpos, const UDim& ypos, const UDim& width, const UDim& height);
		void	SetArea(const UVector2& pos, const UVector2& size);
		void	SetArea(const URect& area);
		void	SetPosition(const UVector2& pos);
		void	SetXPosition(const UDim& x);
		void	SetYPosition(const UDim& y);
		void	SetSize(const UVector2& size);
		void	SetWidth(const UDim& width);
		void	SetHeight(const UDim& height);
		void	SetScale(const Vector3& scale);
		
		// root窗口计算缩放，然后子窗口所有的用父窗口的矩阵
		void	SetActionMatrixScale(const Vector3& scale, bool root = true, const Matrix4* matrix = NULL);

		const GUIString& GetText() { return m_strLogicText; }
		void	SetText(const GUIString& strText);

		GUIFont* GetFont();
		void	SetFont(GUIFont* pFont);

		void	SetVerticalAlignment(VerticalAlignment vertAlign);
		VerticalAlignment GetVerticalAlignment() const { return m_VerticalAlignment; }

		void	SetHorizontalAlignment(HorizontalAlignment horzAlign);
		HorizontalAlignment	GetHorizontalAlignment() const { return m_HorizontalAlignment; }

		Vector2	GetPixelSize() const { return m_PixelSize; }

		const ERect&	GetUnclippedOuterRect() const;//相对全屏的区域
		const ERect&	GetClippedOuterRect() const;//相对全屏的区域
		const ERect&  GetRenderArea();
		// 如果是rt的话，要计算相对于父rt的偏移
		// 如果没有父rt，就是相对于整个屏幕的
		//GUIWindow*	GetParentRT();
		//const ERect	GetRTRenderArea();

		ERect		GetHitTestRect();

		//ERect	GetScreenUnclippedOuterRect() const;
		ERect	GetScreenRenderArea();

		const URect& GetArea() const { return m_Area; }
		//const URect& GetSavedArea() const { return m_SavedArea; }
		//float		GetSavedRotateY() { return m_fSavedRotateY; }

		const UVector2& GetPosition() const;
		const UDim& GetXPosition() const;
		const UDim& GetYPosition() const;

		UDim GetWidth() const;

		UDim GetHeight() const;

		void		SetParent(GUIWindow* parent) { m_pParent = parent; }

		GUIWindow*	GetParent() const { return m_pParent;}

		GUIWindow*	GetTouchableParent();

		void NotifyScreenAreaChanged(bool recursive = true);

		size_t GetChildCount(void) const  {return m_Children.size();}

		GUIWindow* GetChildByIndex(size_t nIndex);
		GUIWindow* GetChildByIndexInDrawlist(size_t nIndex);
		size_t GetDrawlistCount() { return m_DrawList.size(); }

		bool IsAncestor(const GUIWindow* window) const;

		GUIWindow* GetInScroll();

		virtual void OnMoved();

		virtual void OnSized();

		virtual void OnParentSized();

		virtual void OnTextChanged() {
			WindowEventArgs args(this);
			fireEvent(EventWindowTextChanged, args);
		}

		virtual void OnFontChanged() {}

		virtual void OnBeforeDraw(){}
		virtual void OnEndDraw();

		virtual void OnChildAdd(){}
		virtual void OnChildRemove(){}

		void AddChildWindow(const GUIString& strName);

		void AddChildWindow(GUIWindow* window);

		void AddWindowToDrawList(GUIWindow& wnd, bool at_back = false);

		void RemoveWindowFromDrawList(const GUIWindow& wnd);

		void RemoveChildWindow(const GUIString& strName);

		void RemoveChildWindow(GUIWindow* window);

		int GetChildIndex(const GUIString& strName);

		virtual void CleanupChildren(void);

		void ShowOnTop();

		void MoveFront();

		void MoveBack();

		virtual void Destroy(void);

		void Invalidate(void);

		void Invalidate(const bool recursive);


		GUIWindow* GetChildAtPosition(const Vector2& position) const;


		GUIWindow* GetTargetChildAtPosition(const Vector2& position) const;

		virtual bool IsHit(const Vector2& position);

		virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
		virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);	
		virtual void	GetPropertyList(GUIPropertyList& properlist);
		virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);
		//virtual void	GetAnimPropertyList(GUIPropertyList& properlist);

		virtual void	Render();
		void            DrawBackground();

		virtual void DrawSelf();

		virtual void	Update(ui32 nTimeElapse);

		virtual	void	UpdateSelf(ui32 nTimeElapse);

		virtual bool	TouchDown(const Vector2& position);

		virtual bool	TouchUp(const Vector2& position);

		virtual bool	TouchMove(const Vector2& position);

		//virtual bool	TouchMoveOver(const Vector2& position);

		//virtual bool	TouchSlide(const Vector2& position);

		//virtual bool	DoubleClick();

		//virtual bool	OnTouchDrag(const Vector2& position);

		virtual	bool	onDragging(const Touch* touch);
		virtual bool	onDragEnd(const Touch* touch);

		virtual bool	MotionRelease();

		virtual bool	onLongTouchStart(const Touch* touch);
		virtual bool	onLongTouchEnd(const Touch* touch);

		void			ExportXMLData(xml_document<>& doc, xml_node<>* pParentNode);

		void			ImportXMLData(xml_node<>* pNode);

		void			ExportJSONData(rapidjson::Document* doc, rapidjson::Value& node);

		void			ImportJSONData(const rapidjson::Value& node);

		// 从模板创建
		void			ImportTemplateData(const GUIString& strNamePreffix, xml_node<>* pNode);

		// 克隆一个window
		virtual void	Clone(const GUIString& strNamePreffix, GUIWindow* pWindow);

		// 用于ui event 返回dialog name
		void			SetThisWindowDialog() { m_bIsDialog = true; }

		GUIString		GetOwnerDialogName();

		void			SetRotate(float fRotate);
		float			GetRotate() { return m_fRotate; }

		void			SetRotateY(float fRotate);
		float			GetRotateY() { return m_fRotateY; }

		void			SetRotateX(float fRotate);
		float			GetRotateX() { return m_fRotateX; }

		//打开窗口和关闭窗口时播放声音
		void            PlaySoundForWndOpen();
		void            PlaySoundForWndClose();

		// 为了动画效果的缩放，转成相对坐标
		void			ChangeAllChildToReletive();
		void			ToReletive();
		void			ChangeAllChildToAbsolute();
		void			ToAbsolute();

// 		void			SetFlyBeginPoint();
// 		void			SetFlyEndPoint();
		//GUIFlyAnimateController* GetCurentFlyAnim(){return m_pCurentFlyAnim;}
		float			SetEffectName(const GUIString& strEffectName);
		float			PlayEffect();
		float			PlayEffect(const GUIString& strEffectName);
		float			PrepareEffect();
		void			UnprepareEffect();

		void			SetUserData(i32 nID) { m_nUserDataID = nID;  }
		i32				GetUserData() const { return m_nUserDataID;  }

		void			SetUserData2(i32 nID) { m_nUserDataID2 = nID; }
		i32				GetUserData2() const { return m_nUserDataID2; }

		void			setStringUserData(const char* userData) { m_strUserData = userData; }
		const GUIString& getStringUserData() { return m_strUserData; }

		bool			IsOutputLuaVariables() { return m_isOutputLuaVariable; }
		const GUIString& GetLuaEventList() { return m_strLuaEventList; }

		bool			isPlayingAction()  const { return m_isPlayingAction; }
		
		const Matrix4&	getActionMatrix() const { return m_actionMatrix; }

		void			playAction(GUIAction* action, bool endReset = true);

		void			endAction();

		void			setChildPlayAction(bool bPlayed);

		GUIAction* getAction() { return m_action; }

		void			setGray(bool setting);

		void			setEnableLongTouch(bool setting) { m_enableLongTouch = setting; }

		bool			isEnableLongTouch() { return m_enableLongTouch; }

		void			setNeedRenderText() { m_isNeedRenderText = true; }

	protected:

		void			onChildVisibleChange();

		void			checkDisabledMaterial()
		{
			setGray(!IsEnabled());
		}

		// 找父窗口，直到找到一个play 的root的窗口， 通过play接口设置m_isActionRoot
		void			updateActionMatrix(ui32 dt);
		
		GUIWindow*		getActionRoot()
		{
			if (m_isActionRoot)
			{
				return this;
			}
			else if (GetParent())
			{
				return GetParent()->getActionRoot();
			}
			else
			{
				return NULL;
			}
		}

		void	SetArea_impl(const UVector2& pos, const UVector2& size, bool fireEvent = true);

		virtual void AddChild_impl(GUIWindow* wnd);

		virtual void RemoveChild_impl(GUIWindow* wnd);

		void OnLevelChange();

		bool	IsScrollWindow(GUIWindowType eType);

// 		void	BeginRT();
// 		void	EndRT();
		void	RenderBuffer();

		void	UpdateEffectArea();	

		void	getClippedRect(ERect& rect) const;

		// 计算渲染的颜色，考虑alpha
		unsigned int getImageRenderColor()
		{
			m_drawColor.a = GetAlpha();

			return m_drawColor.getABGR();
		}

		const Color& getTextRenderColor()
		{
			m_TextOriColor.a = GetAlpha();

			return m_TextOriColor;
		}

		void changeTouchStatus(bool touchDown, const Vector2& position);

		void SetMaxWidth(int width) { m_maxWidth = width; }
		void SetMinWidth(int width) { m_minWidth = width; }
		void SetMaxHeight(int height) { m_maxHeight = height; }
		void SetMinHeight(int height) { m_minHeight = height; }

	protected:
		typedef vector<GUIWindow*>::type	ChildList;
		typedef map<int,GUIFlyAnimateController*>::type FlyAnimMap;

		GUIWindowType		m_nType;
		GUIString			m_strName;
		//bool				m_bIsRenderTarget;
		//int				m_nRenderTargetID;
		//bool				m_bNeedRefreshRT;
		//GUIString			m_strFuncName;

		bool				m_bEnabled;
		//int 				m_FlyAnimCount;
		bool				m_bVisible;
		bool				m_bActive;
		bool				m_bAutoSize;
		bool				m_bAlwaysOnTop;
		int					m_nLevel;

		// 是否相应触控事件
		bool				m_bIsTouchable;
		ChildList			m_Children;
		ChildList			m_DrawList;
		GUIWindow*			m_pParent;
		URect				m_Area;
		// xml里保存的位置，用于做动画效果
		//URect				m_SavedArea;
		//float				m_fSavedRotateY;

		Vector2				m_PixelSize;
		VerticalAlignment	m_VerticalAlignment;
		HorizontalAlignment	m_HorizontalAlignment;
		mutable ERect		m_OuterUnclippedRect;
		mutable bool		m_OuterUnclippedRectValid;
		mutable ERect		m_ClippedRect;
		mutable bool		m_ClippedRectValid;
		mutable ERect		m_RenderArea;
		mutable bool		m_RenderAreaValid;
		mutable bool		m_NeedsRedraw;

		GUIString			m_strLogicText;
		GUIFont*			m_pFont;
		bool				m_bIsDialog;

		Color               m_drawColor;
		Color               m_TextOriColor;
		Color				m_backgroundColor;
		float				m_fAlpha;
		// 这个用于显示上的裁剪
		bool				m_bIsClipChild;

		//GUIAnimateController*	m_pAnimateController;	
		//FlyAnimMap         m_pFlyAnims;
		//GUIFlyAnimateController* m_pCurentFlyAnim;

		float				m_fRotate;
		float				m_fRotateY;
		float				m_fRotateX;
		bool				m_bMultiClicked;
		bool				m_bTouchSlide;
		ERect				m_rtOldScissor;
		bool				m_bIsOldScissorSet;
		
		bool				m_bEnableColorRect;
		ColorRect			m_ColorRect;
		StretchType			m_StretchType;;
		StretchOffset		m_StretchOffset;

		// 窗口拖拽相关
		bool				m_enableDrag;
		bool				m_dragging;

		//窗口开闭声音
		int					m_nwndCloseSoundName;
		int					m_nwndOpenSoundName;

		//特效系统放到window上，不用增加控件了
		GUIString			m_strEffectName;

		EffectSystem*		m_pEffectSystem;
		SceneNode*			m_pParentNode;
		SceneNode*			m_pSceneNode;
		float				m_fInverseXScale;
		float				m_fInverseYScale;
		float				m_fEffectRotate;
		i32					m_nUserDataID;
		i32					m_nUserDataID2;
		GUIString			m_strUserData;

		// lua
		bool				m_isOutputLuaVariable;
		GUIString			m_strLuaEventList;
		bool				m_enableLongTouch;

		// action
		bool				m_isPlayingAction;
		bool				m_isActionRoot;
		Matrix4				m_actionMatrix;
		GUIAction*			m_action;
		bool				m_actionEndReset;

		// material
		GUIMaterialType		m_material;

		// maskTexture
		GUIString			m_maskTextureName;
		Texture*			m_maskTexture;

		GUIImageset*        m_defaultBackgroundImageset;

		// middle layout chile
		bool				m_isMiddleLayoutChild;
		int                 m_maxWidth;
		int                 m_minWidth;
		int                 m_maxHeight;
		int                 m_minHeight;

		bool				m_isNeedRenderText = false;

		// click and double click related begin
		enum class TouchStatus
		{
			NONE,
			FIRST_DOWN,
			FIRST_UP,
			SECOND_DOWN,
			SECOND_UP
		};
		const static int CLICK_INTERVAL_THRESHOLD;
		ulong m_lastTouchStatusBeginTime	= -1;
		TouchStatus m_currTouchStatus		= TouchStatus::NONE;
		// click and double click related end
	};
}


#endif
