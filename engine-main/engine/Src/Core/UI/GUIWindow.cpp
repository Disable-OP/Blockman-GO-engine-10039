#include "GUIWindow.h"
#include "GUICoordConverter.h"
#include "GUIWindowManager.h"
#include "GUIRenderManager.h"
#include "GUIFontManager.h"
#include "GUIFont.h"
#include "GUISystem.h"
#include "GUIEventArgs.h"
#include "GUIRenderTargetManager.h"
#include "Render/RenderTarget.h"
#include "Audio/SoundSystem.h"
#include "Effect/EffectSystemManager.h"
#include "Render/Material.h"
#include "Touch.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "UI/GUIImagesetManager.h"
#include "UI/GUIImageset.h"
#include <limits>

namespace LORD
{

bool GUIWindow::isEditorMode = false;
const int GUIWindow::CLICK_INTERVAL_THRESHOLD	= 500; // milliseconds

GUIWindow::GUIWindow(GUIWindowType nType, const GUIString& strName)
	: m_nType(nType)
	, m_strName(strName)
	//, m_bIsRenderTarget(false)
	//, m_nRenderTargetID(-1)
	//, m_bNeedRefreshRT(true)
	, m_bEnabled(true)
	//, m_FlyAnimCount(0)
	, m_bVisible(true)
	, m_bActive(false)
	, m_bAutoSize(false)
	, m_bAlwaysOnTop(false)
	, m_nLevel(50)
	, m_bIsTouchable(true)
	, m_pParent(0)
	, m_Area(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(0.0f, 0.0f))
	, m_PixelSize(Vector2::ZERO)
	, m_VerticalAlignment(VA_TOP)
	, m_HorizontalAlignment(HA_LEFT)
	, m_OuterUnclippedRectValid(false)
	, m_ClippedRectValid(false)
	, m_RenderAreaValid(false)
	, m_NeedsRedraw(true)
	, m_pFont(GUIFontManager::Instance()->GetDefaultFont())
	, m_bIsDialog(false)	
	, m_drawColor(Color::WHITE)
	, m_backgroundColor(Color::TRANS)
	, m_bIsClipChild(false)
	//, m_pCurentFlyAnim(NULL)
	, m_fRotate(0.0f)
	, m_fRotateY(0.0f)
	, m_fRotateX(0.0f)
	, m_bMultiClicked(false)
	, m_enableDrag(false)
	, m_dragging(false)
	, m_nwndOpenSoundName(0)
	, m_nwndCloseSoundName(0)
	, m_bTouchSlide( false )
	, m_bIsOldScissorSet(false)
	, m_bEnableColorRect(false)
	, m_StretchType(ST_NONE)
	, m_pEffectSystem(NULL)
	, m_pParentNode(NULL)
	, m_pSceneNode(NULL)
	, m_fInverseXScale(1.0f)
	, m_fInverseYScale(1.0f)
	, m_fEffectRotate(0.0f)
	, m_nUserDataID(-1)
	, m_nUserDataID2(-1)
	, m_strUserData("")
	, m_isOutputLuaVariable(false)
	, m_enableLongTouch(false)
	, m_isPlayingAction(false)
	, m_isActionRoot(false)
	, m_actionMatrix(Matrix4::IDENTITY)
	, m_action(NULL)
	, m_actionEndReset(true)
	, m_material(GUIMaterialType_CULL_BACK)
	, m_maskTexture(NULL)
	, m_isMiddleLayoutChild(false)
	, m_maxWidth(0)
	, m_minWidth(0)
	, m_maxHeight(0)
	, m_minHeight(0)
	, m_defaultBackgroundImageset(GUIImagesetManager::Instance()->createImageset("gui_material.json"))
{
// 	m_pAnimateController = LordNew GUIAnimateController(this);
// 	m_SavedArea = m_Area;
	m_defaultBackgroundImageset->load();
	SetAlpha(1.0f);
}

GUIWindow::~GUIWindow()
{
	if (m_maskTexture)
	{
		// 如果原来有mask，要释放掉
		TextureManager::Instance()->releaseResource(m_maskTexture);
		m_maskTexture = NULL;
	}

	if (m_defaultBackgroundImageset)
	{
		GUIImagesetManager::Instance()->pushbackReleaseImageset(m_defaultBackgroundImageset);
		m_defaultBackgroundImageset = nullptr;
	}

	LordSafeDelete(m_action);

	UnprepareEffect();
	//LordSafeDelete(m_pAnimateController);

// 	FlyAnimMap::iterator it = m_pFlyAnims.begin();
// 	GUIFlyAnimateController* _tmepAnim;
// 	for(;it != m_pFlyAnims.end(); ++it)
// 	{
// 		_tmepAnim = it->second;
// 		LordSafeDelete(_tmepAnim);
// 	}
// 	m_pFlyAnims.clear();
// 	m_pCurentFlyAnim = NULL;
// 
// 	// 释放掉rendertarget
// 	if (m_nRenderTargetID > 0)
// 	{
// 		GUIRenderTargetManager::Instance()->DestroyRenderTarget(m_nRenderTargetID);
// 		m_nRenderTargetID = -1;
// 	}
}

void GUIWindow::SetEnabled( bool bSetting )
{
	if (bSetting != m_bEnabled)
	{
		m_bEnabled = bSetting;

		checkDisabledMaterial();
	}

// 	if (m_bEnabled)
// 		m_drawColor=ENABLED_COLOR;
// 	else
// 		m_drawColor=DISABLED_COLOR;
// 
// 	unsigned char alpha = (unsigned char)(255 * m_fAlpha);
// 
// 	m_drawColor = (m_drawColor & 0x00FFFFFF) | (alpha << 24);
}

void GUIWindow::SetDrawColor(const Color& color)
{
	if (color != m_drawColor)
	{
		m_drawColor = color;
	}
}

void GUIWindow::SetVisible( bool bSetting )
{
	if (bSetting != m_bVisible)
	{
		m_bVisible = bSetting;

		if (m_pParent)
		{
			m_pParent->onChildVisibleChange();
		}
	}
}

void GUIWindow::SetActive( bool bSetting )
{
	if (bSetting != m_bActive)
	{
		m_bActive = bSetting;
	}
}

//----------------------------------------------------------------------------//
void GUIWindow::SetArea_impl(const UVector2& pos, const UVector2& size, bool fireEvent)
{
	m_OuterUnclippedRectValid = false;
	m_ClippedRectValid = false;
	m_RenderAreaValid = false;

	// notes of what we did
	bool moved = false, sized;

	// save original size so we can work out how to behave later on
	const Vector2 oldSize(m_PixelSize);

	const Vector2 baseSize( m_pParent ?
		m_pParent->GetUnclippedOuterRect().getSize():
	GUIRenderManager::Instance()->GetDisplaySize());//父窗口 or 根窗口的大小

	m_PixelSize = size.asAbsolute(baseSize);//基于父窗口的大小，size为相对值
	m_PixelSize.x = Math::Clamp(m_PixelSize.x, (Real)m_minWidth, static_cast<Real>(m_maxWidth ? m_maxWidth : (std::numeric_limits<decltype(m_maxWidth)>::max)()));
	m_PixelSize.y = Math::Clamp(m_PixelSize.y, (Real)m_minHeight, static_cast<Real>(m_maxHeight ? m_maxHeight : (std::numeric_limits<decltype(m_maxHeight)>::max)()));

	m_Area.setSize(size);
	sized = (m_PixelSize != oldSize);

	// only update position if a change has occurred.
	if (pos != m_Area.d_min)
	{
		m_Area.setPosition(pos);
		moved = true;
	}

	if (fireEvent)
	{
		if (moved)
		{
			OnMoved();
		}

		if (sized)
		{
			OnSized();
		}
	}
}

//----------------------------------------------------------------------------//
void GUIWindow::SetArea(const UDim& xpos, const UDim& ypos,
	const UDim& width, const UDim& height)
{
	SetArea(UVector2(xpos, ypos), UVector2(width, height));
}

//----------------------------------------------------------------------------//
void GUIWindow::SetArea(const UVector2& pos, const UVector2& size)
{
	SetArea_impl(pos, size);
}

//----------------------------------------------------------------------------//
void GUIWindow::SetArea(const URect& area)
{
	SetArea(area.d_min, area.getSize());
}

//----------------------------------------------------------------------------//
void GUIWindow::SetPosition(const UVector2& pos)
{
	SetArea_impl(pos, m_Area.getSize());
}

//----------------------------------------------------------------------------//
void GUIWindow::SetXPosition(const UDim& x)
{
	SetArea_impl(UVector2(x, m_Area.d_min.d_y), m_Area.getSize());
}

//----------------------------------------------------------------------------//
void GUIWindow::SetYPosition(const UDim& y)
{
	SetArea_impl(UVector2(m_Area.d_min.d_x, y), m_Area.getSize());
}

//----------------------------------------------------------------------------//
void GUIWindow::SetSize(const UVector2& size)
{
	// set the new size.
	SetArea_impl(m_Area.getPosition(), size);
}

//----------------------------------------------------------------------------//
void GUIWindow::SetWidth(const UDim& width)
{
	SetSize(UVector2(width, m_Area.getSize().d_y));
}

//----------------------------------------------------------------------------//
void GUIWindow::SetHeight(const UDim& height)
{
	SetSize(UVector2(m_Area.getSize().d_x, height));
}

void GUIWindow::SetVerticalAlignment( VerticalAlignment vertAlign )
{
	if (vertAlign != m_VerticalAlignment)
	{
		m_VerticalAlignment = vertAlign;
		NotifyScreenAreaChanged();
	}
}

void GUIWindow::SetHorizontalAlignment( HorizontalAlignment horzAlign )
{
	if (horzAlign != m_HorizontalAlignment)
	{
		m_HorizontalAlignment = horzAlign;
		NotifyScreenAreaChanged();
	}
}

const ERect& GUIWindow::GetUnclippedOuterRect() const//对应于 逻辑绘画区域的绝对位置
{
	if (!m_OuterUnclippedRectValid)
	{
		const ERect local(0, 0, m_PixelSize.x, m_PixelSize.y);
		m_OuterUnclippedRect = CoordConverter::windowToScreen(*this, local);
		m_OuterUnclippedRectValid = true;
	}

	return m_OuterUnclippedRect;
}

const ERect& GUIWindow::GetClippedOuterRect() const//对应于 逻辑绘画区域的绝对位置
{
	if (!m_ClippedRectValid)
	{
		if (IsClipChild())
		{
			m_ClippedRect = GetUnclippedOuterRect();
		}
		else
		{
			m_ClippedRect = ERect(0, 0, (float)GUIRenderManager::Instance()->GetWidth(), (float)GUIRenderManager::Instance()->GetHeight());
		}
		getClippedRect(m_ClippedRect);
		m_ClippedRectValid = true;
	}

	return m_ClippedRect;
}

const ERect& GUIWindow::GetRenderArea()
{
	if (!m_RenderAreaValid)
	{
// 		Vector2 offset;
// 		GetRenderCtxOffset(offset);

		m_RenderArea = GetUnclippedOuterRect();
//		m_RenderArea.offset(-offset);

		m_RenderAreaValid = true;
	}

	return m_RenderArea;
}

// GUIWindow* GUIWindow::GetParentRT()
// {
// 	if (m_pParent)
// 	{
// 		if (m_pParent->m_bIsRenderTarget)
// 		{
// 			return m_pParent;
// 		}
// 		else
// 		{
// 			return m_pParent->GetParentRT();
// 		}
// 	}
// 	else
// 	{
// 		return NULL;
// 	}
// }

// const ERect GUIWindow::GetRTRenderArea()
// {
// 	Vector2 vOffset(Vector2::ZERO);
// 	
// 	GUIWindow* pParentRT = GetParentRT();
// 	if (pParentRT)
// 	{
// 		vOffset.x = pParentRT->GetUnclippedOuterRect().left;
// 		vOffset.y = pParentRT->GetUnclippedOuterRect().top;
// 	}
// 
// 	ERect rtArea = GetUnclippedOuterRect();
// 	rtArea.offset(-vOffset);
// 
// 	return rtArea;
// }

ERect GUIWindow::GetHitTestRect()
{
	if (GetInScroll())
	{
		return GetUnclippedOuterRect().intersect(GetInScroll()->GetUnclippedOuterRect());
	}
	else
	{
		return GetUnclippedOuterRect();
	}
}

// ERect GUIWindow::GetScreenUnclippedOuterRect() const
// {
// 	if (!m_OuterUnclippedRectValid)
// 	{
// 		const ERect local(0, 0, m_PixelSize.x, m_PixelSize.y);
// 		m_OuterUnclippedRect = CoordConverter::windowToScreen(*this, local);
// 		m_OuterUnclippedRectValid = true;
// 	}
// 
// 	ERect ScreenRect;
// 	GUISystem::Instance()->LogicRectToScreenRect(m_OuterUnclippedRect, ScreenRect);
// 	return ScreenRect;
// }

ERect GUIWindow::GetScreenRenderArea()
{
	ERect renderArea = GetRenderArea();
	ERect ScreenRect;
	GUISystem::Instance()->LogicRectToScreenRect(renderArea, ScreenRect);
	return ScreenRect;
}

void GUIWindow::NotifyScreenAreaChanged( bool recursive /*= true*/ )
{
	m_OuterUnclippedRectValid = false;
	m_RenderAreaValid = false;
	m_ClippedRectValid = false;

	if (recursive)
	{
		const size_t child_count = GetChildCount();
		for (size_t i = 0; i < child_count; ++i)
			m_Children[i]->NotifyScreenAreaChanged();
	}
}

void GUIWindow::OnMoved()
{
	NotifyScreenAreaChanged();
}

void GUIWindow::OnSized()
{
	NotifyScreenAreaChanged();

	const size_t child_count = GetChildCount();
	for (size_t i = 0; i < child_count; ++i)
	{
		m_Children[i]->OnParentSized();
	}
}

void GUIWindow::AddChildWindow( const GUIString& strName )
{
	AddChildWindow(GUIWindowManager::Instance()->GetGUIWindow(strName));
}

void GUIWindow::AddChildWindow( GUIWindow* window )
{
	if (!window || window == this)
		return;

	AddChild_impl(window);

	OnChildAdd();
}

void GUIWindow::RemoveChildWindow( const GUIString& strName )
{
	const size_t child_count = GetChildCount();

	for (size_t i = 0; i < child_count; ++i)
	{
		if (m_Children[i]->GetName() == strName)
		{
			RemoveChildWindow(m_Children[i]);
			return;
		}

	}
}

void GUIWindow::RemoveChildWindow( GUIWindow* window )
{
	RemoveChild_impl(window);

	OnChildRemove();
}

int GUIWindow::GetChildIndex(const GUIString & strName)
{
	const size_t child_count = GetChildCount();

	int index = -1;
	for (size_t i = 0; i < child_count; ++i)
	{
		if (m_Children[i]->GetName() == strName)
		{
			//RemoveChildWindow(m_Children[i]);
			index = i;
			return index;
		}
		else
		{
			if (m_Children[i]->GetChildIndex(strName) != -1)
			{
				index = i;
				return index;
			}
		}
	}
	return -1;
}

void GUIWindow::AddChild_impl( GUIWindow* wnd )
{
	// if window is already attached, detach it first (will fire normal events)
	GUIWindow* const old_parent = wnd->GetParent();
	/////////////////////////////////////////////////////
	if (old_parent)
		old_parent->RemoveChildWindow(wnd);
	/////////////////////////////////////////////////////

	AddWindowToDrawList(*wnd);

	// add window to child list
	m_Children.push_back(wnd);

	// set the parent window
	wnd->SetParent(this);

	// update area rects and content for the added window
	wnd->NotifyScreenAreaChanged(true);
	wnd->Invalidate(true);

	if (!old_parent || old_parent->GetPixelSize() != GetPixelSize())
	{
		wnd->OnParentSized();
	}	
}

void GUIWindow::RemoveChild_impl( GUIWindow* wnd )
{
	// remove from draw list
	RemoveWindowFromDrawList(*wnd);

	// if window has no children, we can't remove 'wnd' from the child list
	if (m_Children.empty())
		return;

	// find this window in the child list
	const ChildList::iterator position =
		std::find(m_Children.begin(), m_Children.end(), wnd);

	// if the window was found in the child list
	if (position != m_Children.end())
	{
		// remove window from child list
		m_Children.erase(position);
		// reset windows parent so it's no longer this window.
		wnd->SetParent(0);
	}
}

void GUIWindow::AddWindowToDrawList( GUIWindow& wnd, bool at_back /*= false*/ )
{
	//if (at_back)
	//{
	//	ChildList::iterator pos = m_DrawList.begin();
	//	//if (wnd.isAlwaysOnTop())
	//	//{
	//	//	// find first topmost window
	//	//	while ((pos != m_DrawList.end()) && (!(*pos)->isAlwaysOnTop()))
	//	//		++pos;
	//	//}
	//	// add window to draw list
	//	m_DrawList.insert(pos, &wnd);
	//}
	//// add in front of other windows in group
	//else
	//{
	//	ChildList::reverse_iterator position = m_DrawList.rbegin();
	//	//if (!wnd.isAlwaysOnTop())
	//	//{
	//	//	// find last non-topmost window
	//	//	while ((position != m_DrawList.rend()) && ((*position)->isAlwaysOnTop()))
	//	//		++position;
	//	//}

	//	m_DrawList.insert(position.base(), &wnd);
	//}

	// add behind other windows in same group
	if (at_back)
	{
		// calculate position where window should be added for drawing
		ChildList::iterator pos = m_DrawList.begin();
		while (pos != m_DrawList.end())
		{
			if( (*pos)->GetLevel() <= wnd.GetLevel() )
				break;
			else
				++pos;
		}
		// add window to draw list
		m_DrawList.insert(pos, &wnd);
	}
	// add in front of other windows in group
	else
	{
		// calculate position where window should be added for drawing
		ChildList::reverse_iterator	position = m_DrawList.rbegin();
		if( !wnd.IsAlwaysOnTop() )
		{
			// find last non-topmost window
			while( position != m_DrawList.rend() )
			{
				if( (*position)->GetLevel() < wnd.GetLevel() )
				{
					++position;
				}
				else if( (*position)->GetLevel() == wnd.GetLevel() )
				{
					if( (*position)->IsAlwaysOnTop() )
					{
						++position;
					}
					else
						break;
				}
				else
					break;
			}
		}
		else
		{
			while( position != m_DrawList.rend() )
			{
				if( (*position)->GetLevel() < wnd.GetLevel() )
				{
					++position;
				}
				else
					break;
			}
		}
		// add window to draw list
		m_DrawList.insert(position.base(), &wnd);
	}
}

void GUIWindow::RemoveWindowFromDrawList( const GUIWindow& wnd )
{
	if (!m_DrawList.empty())
	{
		const ChildList::iterator position =
			std::find(m_DrawList.begin(), m_DrawList.end(), &wnd);

		if (position != m_DrawList.end())
			m_DrawList.erase(position);
	}
}

void GUIWindow::Destroy( void )
{
	UnprepareEffect();

	GUIWindowManager* wmgr = GUIWindowManager::Instance();

	if (wmgr->IsGUIWindowPresent(this->GetName()))
	{
		wmgr->DestroyGUIWindow(this);

		return;
	}

	// double check we are detached from parent
	if (m_pParent)
		m_pParent->RemoveChildWindow(this);
	removeAllEvents();
	CleanupChildren();

	MotionRelease();

	Invalidate();
}

void GUIWindow::CleanupChildren( void )
{
	while(GetChildCount() != 0)
	{
		GUIWindow* wnd = m_Children[0];

		// always remove child
		RemoveChildWindow(wnd);

		GUIWindowManager::Instance()->DestroyGUIWindow(wnd);
	}
}

void GUIWindow::Invalidate( void )
{
	//InvalidateRT();
	Invalidate(false);
}

void GUIWindow::Invalidate( const bool recursive )
{
	m_NeedsRedraw = true;

	if (recursive)
	{
		const size_t child_count = GetChildCount();
		for (size_t i = 0; i < child_count; ++i)
			m_Children[i]->Invalidate(true);
	}
}

GUIWindow* GUIWindow::GetTouchableParent()
{
	if (IsTouchable())
	{
		return this;
	}
	else
	{
		if (GetParent())
		{
			return 	GetParent()->GetTouchableParent();
		}
		else
		{
			return NULL;
		}
	}
}

GUIWindow* GUIWindow::GetChildByIndex(size_t nIndex)
{
	LordAssert(nIndex < m_Children.size());
	return m_Children[nIndex];
}

GUIWindow* GUIWindow::GetChildByIndexInDrawlist(size_t nIndex)
{
	LordAssert(nIndex < m_DrawList.size());
	return m_DrawList[nIndex];
}

//获得某点对应的窗口
GUIWindow* GUIWindow::GetChildAtPosition( const Vector2& position ) const
{
	const ChildList::const_reverse_iterator end = m_DrawList.rend();

	ChildList::const_reverse_iterator child;
	for (child = m_DrawList.rbegin(); child != end; ++child)
	{
		if ((*child)->IsVisible())
		{
			GUIWindow* const wnd = (*child)->GetChildAtPosition(position);

			if (wnd)
				return wnd;

			else if ((*child)->IsHit(position))
				return (*child);
		}
	}

	// nothing hit
	return 0;
}

bool GUIWindow::IsHit( const Vector2& position )//判断改点是否在窗口的区域内
{
	if (!IsEnabled())
		return false;

	ERect test_area(GetHitTestRect());

	if ((test_area.getWidth() == 0.0f) || (test_area.getHeight() == 0.0f))
		return false;

	float offset = GUISystem::Instance()->getHitTestOffset();
	test_area.left -= offset;
	test_area.top -= offset;
	test_area.right += offset;
	test_area.bottom += offset;

	return test_area.isPointInRect(position);
}

bool GUIWindow::SetProperty( const GUIString& strName, const GUIString& strValue )
{
	bool bResult = false;
	if (strName == GWP_WINDOW_TYPE)
	{
		// 类型不允许修改
		LordAssert(0);
	}
	else if (strName == GWP_WINDOW_NAME)
	{
		if (GUIWindowManager::Instance()->IsGUIWindowPresent(strValue))
		{
			return true;
		}

		Rename(strValue);
		bResult = true;
	}
// 	else if (strName == GWP_WINDOW_RENDER_TARGET)
// 	{
// 		SetRenderTarget(StringUtil::ParseBool(strValue.c_str()));
// 		bResult = true;
// 	}
	else if (strName == GWP_WINDOW_MATERIAL)
	{
		if (strValue == "CullBack")
		{
			setMaterial(GUIMaterialType_CULL_BACK);
		}
		else if (strValue == "CullNone")
		{
			setMaterial(GUIMaterialType_CULL_NONE);
		}
		else if (strValue == "Mask")
		{
			setMaterial(GUIMaterialType_MASK);
		}
		else if (strValue == "MaskCullNone")
		{
			setMaterial(GUIMaterialType_MASK_CULL_NONE);
		}
		else if (strValue == "CullNoneGray")
		{
			setMaterial(GUIMaterialType_CULL_NONE_GRAY);
		}
		else if (strValue == "CullBackGray")
		{
			setMaterial(GUIMaterialType_CULL_BACK_GRAY);
		}
		else if (strValue == "MaskGray")
		{
			setMaterial(GUIMaterialType_MASK_GRAY);
		}
		else if (strValue == "MaskCullNoneGray")
		{
			setMaterial(GUIMaterialType_MASK_CULL_NONE_GRAY);
		}
		else
		{
			setMaterial(GUIMaterialType_INVALID);
		}
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MASK_TEXTURE)
	{
		setMaskTextureName(strValue);
		bResult = true;
	}
	else if (strName == GWP_WINDOW_DRAW_COLOR)
	{
		SetDrawColor(StringUtil::ParseColor(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLED)
	{
		SetEnabled(StringUtil::ParseBool(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_AREA)
	{
		SetArea(StringUtil::ParseURect(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MAX_WIDTH)
	{
		SetMaxWidth(StringUtil::ParseInt(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIN_WIDTH)
	{
		SetMinWidth(StringUtil::ParseInt(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MAX_HEIGHT)
	{
		SetMaxHeight(StringUtil::ParseInt(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIN_HEIGHT)
	{
		SetMinHeight(StringUtil::ParseInt(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_VA)
	{
		VerticalAlignment align;
		if (strValue == "Centre")
		{
			align = VA_CENTRE;
		}
		else if (strValue == "Bottom")
		{
			align = VA_BOTTOM;
		}
		else
		{
			align = VA_TOP;
		}
		SetVerticalAlignment(align);
		bResult = true;
	}
	else if (strName == GWP_WINDOW_HA)
	{
		HorizontalAlignment align;
		if (strValue == "Centre")
		{
			align = HA_CENTRE;
		}
		else if (strValue == "Right")
		{
			align = HA_RIGHT;
		}
		else
		{
			align = HA_LEFT;
		}
		SetHorizontalAlignment(align);
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT)
	{
		SetText(strValue);
		bResult = true;
	}
	else if (strName == GWP_WINDOW_FONT)
	{
		GUIFont* pFont = GUIFontManager::Instance()->GetFont(strValue);
		if (pFont)
		{
			SetFont(pFont);
		}
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TOUCHABLE)
	{
		SetTouchable(StringUtil::ParseBool(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_VISIBLE)
	{
		SetVisible(StringUtil::ParseBool(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_AUTOSIZE)
	{
		SetAutoSize(StringUtil::ParseBool(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_LEVEL)
	{
		SetLevel(StringUtil::ParseInt(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ALWAYS_ON_TOP)
	{
		SetAlwaysOnTop(StringUtil::ParseBool(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_BACKGROUND_COLOR)
	{
		SetBackgroundColor(StringUtil::ParseColor(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ALPHA)
	{
		SetAlpha(StringUtil::ParseFloat(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIDDLE_LAYOUT_CHILD)
	{
		m_isMiddleLayoutChild = StringUtil::ParseBool(strValue.c_str());

		onChildVisibleChange();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_CLIPCHILD)
	{
		SetClipChild(StringUtil::ParseBool(strValue.c_str()));
		bResult = true;
	}
// 	else if(strName == GWP_WINDOW_FLY_ANIM_COUNT)
// 	{
// 		SetFlyAnimCount(StringUtil::ParseInt(strValue.c_str()));
// 		bResult = true;
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIM_SELECT)
// 	{
// 		StringArray stra = StringUtil::Split(strValue.c_str(), "_");
// 		int _index = StringUtil::ParseInt(stra.at(1));
// 		SetSelectFlyAnim(_index);
// 		bResult = true;
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_ACCELERATION)
// 	{
// 		if(m_pCurentFlyAnim)
// 		{
// 			m_pCurentFlyAnim->GetFlyAnimatInfo().f_Acceleration = StringUtil::ParseFloat(strValue.c_str());
// 			bResult = true;
// 		}
// 
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_ACC_DIR)
// 	{
// 		if(m_pCurentFlyAnim)
// 		{
// 			int type = 0;;
// 			if (strValue == "Left(Uniform)")
// 			{
// 				type = 1;
// 			}
// 			else if (strValue == "Right(Acceleration)")
// 			{
// 				type = 2;
// 			}
// 			else if (strValue == "Random(Deceleration)")
// 			{
// 				type = 3;
// 			}
// 			m_pCurentFlyAnim->GetFlyAnimatInfo().f_AccDir = type;
// 			bResult = true;
// 		}
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_TIME)
// 	{
// 		if(m_pCurentFlyAnim)
// 		{
// 			m_pCurentFlyAnim->GetFlyAnimatInfo().f_time = StringUtil::ParseUI32(strValue.c_str());
// 			bResult = true;
// 		}
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_TYPE)
// 	{
// 		if(m_pCurentFlyAnim)
// 		{
// 			GUIFlyAnimateController::FlyAnmateType _type = GUIFlyAnimateController::FLY_AT_STRAIGHTLINE;
// 			if (strValue == "Straightline")
// 			{
// 				_type = GUIFlyAnimateController::FLY_AT_STRAIGHTLINE;
// 			}
// 			else if (strValue == "Parabola")
// 			{
// 				_type = GUIFlyAnimateController::FLY_AT_PARABOLA;
// 			}
// 			else if (strValue == "Missile")
// 			{
// 				_type = GUIFlyAnimateController::FLY_AT_MISSILE;
// 			}
// 			m_pCurentFlyAnim->GetFlyAnimatInfo().f_type = _type;
// 			bResult = true;
// 		}
// 	}
// 	else if (strName == GWP_WINDOW_ANIMATE_TYPE)
// 	{
// 		GUIAnimateController::AnimateType type;
// 		if (strValue == "FromLeft")
// 		{
// 			type = GUIAnimateController::AT_FLY_FROM_LEFT;
// 		}
// 		else if (strValue == "FromTop")
// 		{
// 			type = GUIAnimateController::AT_FLY_FROM_TOP;
// 		}
// 		else if (strValue == "FromRight")
// 		{
// 			type = GUIAnimateController::AT_FLY_FROM_RIGHT;
// 		}
// 		else if (strValue == "FromBottom")
// 		{
// 			type = GUIAnimateController::AT_FLY_FROM_BOTTOM;
// 		}
// 		else if (strValue == "Popup")
// 		{
// 			type = GUIAnimateController::AT_POPUP;
// 		}
// 		else if (strValue == "Rotate")
// 		{
// 			type = GUIAnimateController::AT_ROTATE;
// 		}
// 		else
// 		{
// 			type = GUIAnimateController::AT_NONE;
// 		}
// 
// 		SetAnimateType(type);
// 		bResult = true;
// 	}
	else if (strName == GWP_WINDOW_ROTATE)
	{
		SetRotate(StringUtil::ParseFloat(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ROTATEY)
	{
		SetRotateY(StringUtil::ParseFloat(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ROTATEX)
	{
		SetRotateX(StringUtil::ParseFloat(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_OPEN_SOUND)
	{
		m_nwndOpenSoundName = StringUtil::ParseInt(strValue.c_str(), 0);
		bResult = true;
	}
	else if (strName == GWP_WINDOW_CLOSE_SOUND)
	{
		m_nwndCloseSoundName = StringUtil::ParseInt(strValue.c_str(), 0);
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MULTI_CLICK)
	{
		SetMultiClicked(StringUtil::ParseBool(strValue.c_str()));
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLE_TEXT_COLOR_RECT)
	{
		m_bEnableColorRect = StringUtil::ParseBool(strValue.c_str());
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_LT)
	{
		m_ColorRect.cLeftTop = StringUtil::ParseColor(strValue.c_str());
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_RT)
	{
		m_ColorRect.cRightTop = StringUtil::ParseColor(strValue.c_str());
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_LB)
	{
		m_ColorRect.cLeftBottom = StringUtil::ParseColor(strValue.c_str());
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_RB)
	{
		m_ColorRect.cRightBottom = StringUtil::ParseColor(strValue.c_str());
		bResult = true;
	}
	else if (strName == GWP_WINDOW_STRETCH_TYPE)
	{
		if (strValue == "None")
		{
			m_StretchType = ST_NONE;
		}
		else if (strValue == "LeftRight")
		{
			m_StretchType = ST_LEFT_RIGHT;
		}
		else if (strValue == "TopBottom")
		{
			m_StretchType = ST_TOP_BOTTOM;
		}
		else if (strValue == "NineGrid")
		{
			m_StretchType = ST_NINE;
		}

		bResult = true;
	}
	else if (strName == GWP_WINDOW_STRETCH_OFFSET)
	{
		Vector4 temp = StringUtil::ParseVec4(strValue.c_str());

		m_StretchOffset.fLeft = temp.x;
		m_StretchOffset.fRight = temp.y;
		m_StretchOffset.fTop = temp.z;
		m_StretchOffset.fBottom = temp.w;

		bResult = true;
	}
	else if (strName == GWP_WINDOW_EFFECT_NAME)
	{
		SetEffectName(strValue);
		bResult = true;
	}
	else if (strName == GWP_WINDOW_EFFECT_ROTATE)
	{
		m_fEffectRotate = StringUtil::ParseFloat(strValue.c_str());
		bResult = true;
	}
	else if ( strName == GWP_WINDOW_LUA_VARIABLE)
	{
		m_isOutputLuaVariable = StringUtil::ParseBool(strValue.c_str());
		bResult = true;
	}
	else if ( strName == GWP_WINDOW_LUA_EVENT)
	{
		m_strLuaEventList = strValue;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLE_LONG_TOUCH)
	{
		m_enableLongTouch = StringUtil::ParseBool(strValue.c_str());
		bResult = true;
	}
	else
	{

	}

	return bResult;
}

bool GUIWindow::GetProperty( const GUIString& strName, GUIString& strValue)
{
	bool bResult = false;

	if (strName == GWP_WINDOW_TYPE)
	{
		strValue = m_nType.value;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_NAME)
	{
		strValue = m_strName;
		bResult = true;
	}
// 	else if (strName == GWP_WINDOW_RENDER_TARGET)
// 	{
// 		strValue = StringUtil::ToString(m_bIsRenderTarget).c_str();
// 		bResult = true;
// 	}
	else if (strName == GWP_WINDOW_MATERIAL)
	{
		GUIMaterialType material = getMaterial();
		if (material == GUIMaterialType_CULL_BACK)
		{
			strValue = "CullBack";
		}
		else if (material == GUIMaterialType_CULL_NONE)
		{
			strValue = "CullNone";
		}
		else if (material == GUIMaterialType_MASK)
		{
			strValue = "Mask";
		}
		else if (material == GUIMaterialType_MASK_CULL_NONE)
		{
			strValue = "MaskCullNone";
		}
		else if (material == GUIMaterialType_CULL_BACK_GRAY)
		{
			strValue = "CullBackGray";
		}
		else if (material == GUIMaterialType_CULL_NONE_GRAY)
		{
			strValue = "CullNoneGray";
		}
		else if (material == GUIMaterialType_MASK_GRAY)
		{
			strValue = "MaskGray";
		}
		else if (material == GUIMaterialType_MASK_CULL_NONE_GRAY)
		{
			strValue = "MaskCullNoneGray";
		}
		else
		{
			strValue = "";
		}
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MASK_TEXTURE)
	{
		strValue = getMaskTextureName();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_DRAW_COLOR)
	{
		strValue = StringUtil::ToString(GetDrawColor()).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLED)
	{
		strValue = StringUtil::ToString(m_bEnabled).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_AREA)
	{
		strValue = StringUtil::ToString(m_Area).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MAX_WIDTH)
	{
		strValue = StringUtil::ToString(m_maxWidth).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIN_WIDTH)
	{
		strValue = StringUtil::ToString(m_minWidth).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MAX_HEIGHT)
	{
		strValue = StringUtil::ToString(m_maxHeight).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIN_HEIGHT)
	{
		strValue = StringUtil::ToString(m_minHeight).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_VA)
	{
		switch(m_VerticalAlignment)
		{
		case VA_CENTRE:
			strValue = "Centre";
			break;

		case VA_BOTTOM:
			strValue = "Bottom";
			break;

		default:
			strValue = "Top";
		}
		bResult = true;
	}
	else if (strName == GWP_WINDOW_HA)
	{
		switch(m_HorizontalAlignment)
		{
		case HA_CENTRE:
			strValue = "Centre";
			break;

		case HA_RIGHT:
			strValue = "Right";
			break;

		default:
			strValue = "Left";
		}
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT)
	{
		strValue = GetText();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_FONT)
	{
		if (m_pFont)
		{
			strValue = m_pFont->GetName();
		}
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TOUCHABLE)
	{
		strValue = StringUtil::ToString(IsTouchable()).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_VISIBLE)
	{
		strValue = StringUtil::ToString(IsVisible()).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_AUTOSIZE)
	{
		strValue = StringUtil::ToString(IsAutoSize()).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_LEVEL)
	{
		strValue = StringUtil::ToString(GetLevel()).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ALWAYS_ON_TOP)
	{
		strValue = StringUtil::ToString(IsAlwaysOnTop()).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_BACKGROUND_COLOR)
	{
		strValue = StringUtil::ToString(m_backgroundColor).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ALPHA)
	{
		strValue = StringUtil::ToString(m_fAlpha).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_CLIPCHILD)
	{
		strValue = StringUtil::ToString(m_bIsClipChild).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIDDLE_LAYOUT_CHILD)
	{
		strValue = StringUtil::ToString(m_isMiddleLayoutChild).c_str();
		bResult = true;
	}
// 	else if(strName == GWP_WINDOW_FLY_ANIM_COUNT)
// 	{
// 		strValue = StringUtil::ToString(getFlyAnimCount()).c_str();
// 		bResult = true;
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIM_SELECT)
// 	{
// 		int _index = GetSelectFlyAnim();
// 		if(_index)
// 		{
// 			strValue = ("Anim_" + StringUtil::ToString(_index)).c_str();
// 		}
// 		else
// 		{
// 			strValue = "None";
// 		}
// 		bResult = true;
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_ACCELERATION)
// 	{
// 		if(m_pCurentFlyAnim)
// 		{
// 			float acc = m_pCurentFlyAnim->GetFlyAnimatInfo().f_Acceleration;
// 			strValue = StringUtil::ToString(acc).c_str();
// 			bResult = true;
// 		}
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_ACC_DIR)
// 	{
// 		if (m_pCurentFlyAnim)
// 		{
// 			int type = m_pCurentFlyAnim->GetFlyAnimatInfo().f_AccDir;
// 			if (type == 1 )
// 			{
// 				strValue = "Left(Uniform)";
// 			}
// 			else if (type == 2 )
// 			{
// 				strValue = "Right(Acceleration)";
// 			}
// 			else if (type == 3 )
// 			{
// 				strValue = "Random(Deceleration)";
// 			}
// 			bResult = true;
// 		}
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_TIME)
// 	{
// 		if (m_pCurentFlyAnim)
// 		{
// 			strValue = StringUtil::ToString(m_pCurentFlyAnim->GetFlyAnimatInfo().f_time).c_str();
// 			bResult = true;
// 		}	
// 	}
// 	else if(strName == GWP_WINDOW_FLY_ANIMATE_TYPE)
// 	{
// 		if (m_pCurentFlyAnim)
// 		{
// 			if (m_pCurentFlyAnim->GetFlyAnimatInfo().f_type == GUIFlyAnimateController::FLY_AT_STRAIGHTLINE)
// 			{
// 				strValue = "Straightline";
// 			}
// 			else if (m_pCurentFlyAnim->GetFlyAnimatInfo().f_type == GUIFlyAnimateController::FLY_AT_PARABOLA)
// 			{
// 				strValue = "Parabola";
// 			}
// 			else if (m_pCurentFlyAnim->GetFlyAnimatInfo().f_type == GUIFlyAnimateController::FLY_AT_MISSILE)
// 			{
// 				strValue = "Missile";
// 			}
// 		}
// 		bResult = true;
// 	}
// 	else if (strName == GWP_WINDOW_ANIMATE_TYPE)
// 	{
// 		if (m_pAnimateController)
// 		{
// 			if (m_pAnimateController->GetAnimateType() == GUIAnimateController::AT_FLY_FROM_LEFT)
// 			{
// 				strValue = "FromLeft";
// 			}
// 			else if (m_pAnimateController->GetAnimateType() == GUIAnimateController::AT_FLY_FROM_TOP)
// 			{
// 				strValue = "FromTop";
// 			}
// 			else if (m_pAnimateController->GetAnimateType() == GUIAnimateController::AT_FLY_FROM_RIGHT)
// 			{
// 				strValue = "FromRight";
// 			}
// 			else if (m_pAnimateController->GetAnimateType() == GUIAnimateController::AT_FLY_FROM_BOTTOM)
// 			{
// 				strValue = "FromBottom";
// 			}
// 			else if (m_pAnimateController->GetAnimateType() == GUIAnimateController::AT_POPUP)
// 			{
// 				strValue = "Popup";
// 			}
// 			else if (m_pAnimateController->GetAnimateType() == GUIAnimateController::AT_ROTATE)
// 			{
// 				strValue = "Rotate";
// 			}
// 			else
// 			{
// 				strValue = "None";
// 			}
// 		}
// 		bResult = true;
// 	}
	else if (strName == GWP_WINDOW_ROTATE)
	{
		strValue = StringUtil::ToString(m_fRotate).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ROTATEY)
	{
		strValue = StringUtil::ToString(m_fRotateY).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ROTATEX)
	{
		strValue = StringUtil::ToString(m_fRotateX).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_OPEN_SOUND)
	{
		strValue = StringUtil::ToString(m_nwndOpenSoundName).c_str();	
		bResult = true;
	}
	else if (strName == GWP_WINDOW_CLOSE_SOUND)
	{
		strValue = StringUtil::ToString(m_nwndCloseSoundName).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MULTI_CLICK)
	{
		strValue = StringUtil::ToString(m_bMultiClicked).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLE_TEXT_COLOR_RECT)
	{
		strValue = StringUtil::ToString(m_bEnableColorRect).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_LT)
	{
		strValue = StringUtil::ToString(m_ColorRect.cLeftTop).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_RT)
	{
		strValue = StringUtil::ToString(m_ColorRect.cRightTop).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_LB)
	{
		strValue = StringUtil::ToString(m_ColorRect.cLeftBottom).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_RB)
	{
		strValue = StringUtil::ToString(m_ColorRect.cRightBottom).c_str();
		bResult = true;
	}
	else if (strName == GWP_WINDOW_STRETCH_TYPE)
	{
		if (m_StretchType == ST_NONE)
		{
			strValue = "None";
		}
		else if (m_StretchType == ST_LEFT_RIGHT)
		{
			strValue = "LeftRight";
		}
		else if (m_StretchType == ST_TOP_BOTTOM)
		{
			strValue = "TopBottom";
		}
		else if (m_StretchType == ST_NINE)
		{
			strValue = "NineGrid";
		}

		bResult = true;
	}
	else if (strName == GWP_WINDOW_STRETCH_OFFSET)
	{
		Vector4 temp;

		temp.x = m_StretchOffset.fLeft;
		temp.y = m_StretchOffset.fRight;
		temp.z = m_StretchOffset.fTop;
		temp.w = m_StretchOffset.fBottom;

		strValue = StringUtil::ToString(temp).c_str();

		bResult = true;
	}
	if (strName == GWP_WINDOW_EFFECT_NAME)
	{
		strValue = m_strEffectName;
		return true;
	}
	else if (strName == GWP_WINDOW_EFFECT_ROTATE)
	{
		strValue = StringUtil::ToString(m_fEffectRotate).c_str();
		return true;
	}
	else if (strName == GWP_WINDOW_LUA_VARIABLE)
	{
		strValue = StringUtil::ToString(m_isOutputLuaVariable).c_str();
		return true;
	}
	else if (strName == GWP_WINDOW_LUA_EVENT)
	{
		strValue = m_strLuaEventList;
		return true;
	}
	else if (strName == GWP_WINDOW_ENABLE_LONG_TOUCH)
	{
		strValue = StringUtil::ToString(m_enableLongTouch).c_str();
		bResult = true;
	}
	else
	{

	}

	return bResult;
}

void GUIWindow::GetPropertyList( GUIPropertyList& properlist )
{
	properlist.push_back(GWP_WINDOW_TYPE);
	properlist.push_back(GWP_WINDOW_NAME);
	properlist.push_back(GWP_WINDOW_MATERIAL);
	properlist.push_back(GWP_WINDOW_MASK_TEXTURE);
	properlist.push_back(GWP_WINDOW_DRAW_COLOR);
	properlist.push_back(GWP_WINDOW_ENABLED);
	properlist.push_back(GWP_WINDOW_AREA);
	properlist.push_back(GWP_WINDOW_MAX_WIDTH);
	properlist.push_back(GWP_WINDOW_MIN_WIDTH);
	properlist.push_back(GWP_WINDOW_MAX_HEIGHT);
	properlist.push_back(GWP_WINDOW_MIN_HEIGHT);
	properlist.push_back(GWP_WINDOW_VA);
	properlist.push_back(GWP_WINDOW_HA);
	properlist.push_back(GWP_WINDOW_TEXT);
	properlist.push_back(GWP_WINDOW_FONT);
	properlist.push_back(GWP_WINDOW_TOUCHABLE);
	properlist.push_back(GWP_WINDOW_VISIBLE);
	properlist.push_back(GWP_WINDOW_AUTOSIZE);
	properlist.push_back(GWP_WINDOW_LEVEL);
	properlist.push_back(GWP_WINDOW_ALWAYS_ON_TOP);
	properlist.push_back(GWP_WINDOW_BACKGROUND_COLOR);
	properlist.push_back(GWP_WINDOW_ALPHA);
	properlist.push_back(GWP_WINDOW_CLIPCHILD);
// 	properlist.push_back(GWP_WINDOW_ANIMATE_TYPE);
// 	properlist.push_back(GWP_WINDOW_FLY_ANIM_COUNT);
	properlist.push_back(GWP_WINDOW_ROTATE);
	properlist.push_back(GWP_WINDOW_ROTATEY);
	properlist.push_back(GWP_WINDOW_ROTATEX);
	properlist.push_back(GWP_WINDOW_OPEN_SOUND);
	properlist.push_back(GWP_WINDOW_CLOSE_SOUND);
	properlist.push_back(GWP_WINDOW_MULTI_CLICK);
//	properlist.push_back(GWP_WINDOW_RENDER_TARGET);
	properlist.push_back(GWP_WINDOW_ENABLE_TEXT_COLOR_RECT);
	properlist.push_back(GWP_WINDOW_TEXT_COLOR_LT);
	properlist.push_back(GWP_WINDOW_TEXT_COLOR_RT);
	properlist.push_back(GWP_WINDOW_TEXT_COLOR_LB);
	properlist.push_back(GWP_WINDOW_TEXT_COLOR_RB);
	properlist.push_back(GWP_WINDOW_STRETCH_TYPE);
	properlist.push_back(GWP_WINDOW_STRETCH_OFFSET);
	properlist.push_back(GWP_WINDOW_EFFECT_NAME);
	properlist.push_back(GWP_WINDOW_EFFECT_ROTATE);
	properlist.push_back(GWP_WINDOW_LUA_VARIABLE);
	properlist.push_back(GWP_WINDOW_LUA_EVENT);
	properlist.push_back(GWP_WINDOW_MIDDLE_LAYOUT_CHILD);
	properlist.push_back(GWP_WINDOW_ENABLE_LONG_TOUCH);
}

bool GUIWindow::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
{
	bool bResult = false;
	if (strName == GWP_WINDOW_TYPE)
	{
		type = GPT_STRING;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_NAME)
	{
		type = GPT_STRING;
		bResult = true;
	}
// 	else if (strName == GWP_WINDOW_RENDER_TARGET)
// 	{
// 		type = GPT_BOOL;
// 		bResult = true;
// 	}
	else if (strName == GWP_WINDOW_MATERIAL)
	{
		type = GPT_MATERIAL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MASK_TEXTURE)
	{
		type = GPT_STRING;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_DRAW_COLOR)
	{
		type = GPT_COLOR;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLED)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_AREA)
	{
		type = GPT_AREA;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MAX_WIDTH)
	{
		type = GPT_INT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIN_WIDTH)
	{
		type = GPT_INT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MAX_HEIGHT)
	{
		type = GPT_INT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIN_HEIGHT)
	{
		type = GPT_INT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_VA)
	{
		type = GPT_VA;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_HA)
	{
		type = GPT_HA;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT)
	{
		type = GPT_STRING;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_FONT)
	{
		type = GPT_FONT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TOUCHABLE)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_VISIBLE)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_AUTOSIZE)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_LEVEL)
	{
		type = GPT_INT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ALWAYS_ON_TOP)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_BACKGROUND_COLOR)
	{
		type = GPT_COLOR;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ALPHA)
	{
		type = GPT_FLOAT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_CLIPCHILD)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MIDDLE_LAYOUT_CHILD)
	{
		type = GPT_BOOL;
		bResult = true;
	}
// 	else if (strName == GWP_WINDOW_ANIMATE_TYPE)
// 	{
// 		type = GPT_ANIMATE_TYPE;
// 		bResult = true;
// 	}
// 	else if (strName == GWP_WINDOW_FLY_ANIM_COUNT)
// 	{
// 		type = GPT_INT;
// 		bResult = true;
// 	}
// 	else if (strName == GWP_WINDOW_FLY_ANIM_SELECT)
// 	{
// 		type = GPT_SELECT_FLY_ANIME;
// 		bResult = true;
// 	}
// 	else if (strName == GWP_WINDOW_FLY_ANIMATE_TYPE)
// 	{
// 		type = GPT_FLY_ANIMATE_TPYE;
// 		bResult = true;
// 	}
// 	else if (strName == GWP_WINDOW_FLY_ANIMATE_ACCELERATION)
// 	{
// 		type = GPT_FLOAT;
// 		bResult = true;
// 	}
// 	else if (strName == GWP_WINDOW_FLY_ANIMATE_ACC_DIR)
// 	{
// 		type = GPT_FLY_ACC_DIR;
// 		bResult = true;
// 	}
// 	else if (strName == GWP_WINDOW_FLY_ANIMATE_TIME)
// 	{
// 		type = GPT_INT;
// 		bResult = true;
// 	}
	else if (strName == GWP_WINDOW_ROTATE)
	{
		type = GPT_FLOAT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ROTATEY)
	{
		type = GPT_FLOAT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ROTATEX)
	{
		type = GPT_FLOAT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_OPEN_SOUND)
	{
		type = GPT_STRING;
		bResult = true;
	}	
	else if (strName == GWP_WINDOW_CLOSE_SOUND)
	{
		type = GPT_STRING;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_MULTI_CLICK)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLE_TEXT_COLOR_RECT)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_LT)
	{
		type = GPT_COLOR;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_RT)
	{
		type = GPT_COLOR;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_LB)
	{
		type = GPT_COLOR;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_TEXT_COLOR_RB)
	{
		type = GPT_COLOR;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_STRETCH_TYPE)
	{
		type = GPT_STRETCH_TYPE;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_STRETCH_OFFSET)
	{
		type = GPT_STRETCH_OFFSET;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_EFFECT_NAME)
	{
		type = GPT_STRING;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_EFFECT_ROTATE)
	{
		type = GPT_FLOAT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_LUA_VARIABLE)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_LUA_EVENT)
	{
		type = GPT_LUA_EVENT;
		bResult = true;
	}
	else if (strName == GWP_WINDOW_ENABLE_LONG_TOUCH)
	{
		type = GPT_BOOL;
		bResult = true;
	}
	else
	{
	}
	
	return bResult;
}

void GUIWindow::Render()
{
	if (!IsVisible())
	{
		return;
	}

	// 判断是否被裁减了，如果被裁减了就不提交渲染
	GUIWindow* pScrollWindow = GetInScroll();
	if (pScrollWindow)
	{
		ERect rtHit = GetHitTestRect();
		if ((rtHit.getWidth() == 0.0f) || (rtHit.getHeight() == 0.0f))
		{
			return;
		}
	}

	// 判断是否裁剪子控件
	if (m_bIsClipChild /*&& !m_bIsRenderTarget*/ && !isPlayingAction())
	{
		ERect rtClipRect = GetScreenRenderArea();

		if (GUIRenderManager::Instance()->IsInScissor())
		{
			m_rtOldScissor = GUIRenderManager::Instance()->GetCurrentScissor();
			rtClipRect = rtClipRect.intersect(m_rtOldScissor);
			m_bIsOldScissorSet = true;
		} 
		else
		{
			m_rtOldScissor.setNull();
			m_bIsOldScissorSet = false;
		}

		GUIRenderManager::Instance()->BeginScissorRect(rtClipRect);
	}

// 	if (m_bIsRenderTarget)
// 	{
// 		if (m_bNeedRefreshRT)
// 		{
// 			// 开始处理rt
// 			BeginRT();
// 
// 			RenderBuffer();
// 
// 			// 结束rt
// 			EndRT();
// 		}
// 		else
// 		{
// 			// 只是刷上去
// 			EndRT();
// 		}
// 	}
// 	else
//	{
		RenderBuffer();
//	}

		if (m_bIsClipChild /*&& !m_bIsRenderTarget*/ && !isPlayingAction())
	{
		if (m_bIsOldScissorSet)
		{
			GUIRenderManager::Instance()->BeginScissorRect(m_rtOldScissor);
		} 
		else
		{
			GUIRenderManager::Instance()->EndScissorRect();
		}
	}
}

void GUIWindow::DrawSelf()
{

}

void GUIWindow::SetText( const GUIString& strText )
{
	if (strText != m_strLogicText)
	{
		m_strLogicText = strText;
		OnTextChanged();
		Invalidate();
	}
}

GUIFont* GUIWindow::GetFont()
{
	return m_pFont;
}

void GUIWindow::SetFont( GUIFont* pFont )
{
	if (pFont != m_pFont)
	{
		m_pFont = pFont;
		Invalidate();
		OnFontChanged();
	}
}

bool GUIWindow::IsAncestor( const GUIWindow* window ) const
{
	// if we have no parent, then return false
	if (!m_pParent)
		return false;

	// check our immediate parent
	if (m_pParent == window)
		return true;

	// not our parent, check back up the family line
	return m_pParent->IsAncestor(window);
}

void GUIWindow::changeTouchStatus(bool touchDown, const Vector2& position)
{
	MouseEventArgs args(this);
	args.position = position;
	ulong currTime = Time::Instance()->getMilliseconds();

	switch (m_currTouchStatus) {
	case TouchStatus::NONE:
		if (touchDown) {
			m_currTouchStatus = TouchStatus::FIRST_DOWN;
			m_lastTouchStatusBeginTime = currTime;
		}
		break;

	case TouchStatus::FIRST_DOWN:
		if (!touchDown) {
			if (currTime - m_lastTouchStatusBeginTime < CLICK_INTERVAL_THRESHOLD) {
				m_currTouchStatus = TouchStatus::FIRST_UP;
				m_lastTouchStatusBeginTime = currTime;
				fireEvent(EventWindowClick, args);
			}
			else {
				m_currTouchStatus = TouchStatus::NONE;
			}
		}
		else {
			m_currTouchStatus = TouchStatus::NONE;
		}
		break;

	case TouchStatus::FIRST_UP:
		if (touchDown) {
			if (currTime - m_lastTouchStatusBeginTime < CLICK_INTERVAL_THRESHOLD) 
			{
				m_currTouchStatus = TouchStatus::SECOND_DOWN;
			}
			else 
			{
				m_currTouchStatus = TouchStatus::FIRST_DOWN;
			}
			m_lastTouchStatusBeginTime = currTime;
		}
		else {
			m_currTouchStatus = TouchStatus::NONE;
		}
		break;

	case TouchStatus::SECOND_DOWN:
		if (!touchDown) {
			if (currTime - m_lastTouchStatusBeginTime < CLICK_INTERVAL_THRESHOLD) 
			{
				fireEvent(EventWindowClick, args);
				fireEvent(EventWindowDoubleClick, args);
			}
		}
		m_currTouchStatus = TouchStatus::NONE;
		break;

	default:
		// should not go here
		LordAssert(false);
		break;
	}
}

bool GUIWindow::TouchDown( const Vector2& position )
{
	//GUISystem::Instance()->GetEventCallback()(GET_WINDOW_TOUCH_DOWN, GetOwnerDialogName(), GetName(), 0, 0);
	changeTouchStatus(true, position);

	MouseEventArgs args(this);
	args.position = position;
	fireEvent(EventWindowTouchDown, args);
	if (m_pParent)
	{
		m_pParent->TouchDown(position);
	}
	return true;
}

bool GUIWindow::TouchUp( const Vector2& position )
{
	//GUISystem::Instance()->GetEventCallback()(GET_WINDOW_TOUCH_UP, GetOwnerDialogName(), GetName(), 0, 0);
	changeTouchStatus(false, position);

	MouseEventArgs args(this);
	args.position = position;
	fireEvent(EventWindowTouchUp, args);
	if (m_pParent)
	{
		m_pParent->TouchUp(position);
	}
	return true;
}

bool GUIWindow::TouchMove( const Vector2& position )
{
	MouseEventArgs args(this);
	args.position = position;
	fireEvent(EventWindowTouchMove, args);
	if (m_pParent)
	{
		m_pParent->TouchMove(position);
	}
	return true;
}

// bool GUIWindow::TouchMoveOver(const Vector2& position)
// {
// 	WindowEventArgs args(this);
// 	fireEvent(EventWindowTouchMoveOver, args);
// 
// 	return true;
// }


// bool	GUIWindow::TouchSlide(const Vector2& position)
// {
// 	//GUISystem::Instance()->GetEventCallback()( GET_WINDOW_TOUCH_SLIDE, GetOwnerDialogName(), GetName(), 0, 0 );
// 
// 	return true;
// }

// bool GUIWindow::DoubleClick()
// {
// 	WindowEventArgs args(this);
// 	fireEvent( EventWindowDoubleClick, args );
// 
// 	//GUISystem::Instance()->GetEventCallback()(GET_WINDOW_DOUBLE_CLICK, GetOwnerDialogName(), GetName(), 0, 0);
// 	return true;
// }

bool GUIWindow::MotionRelease()
{
	WindowEventArgs args(this);
	fireEvent(EventMotionRelease, args);
	if (m_pParent)
	{
		m_pParent->MotionRelease();
	}
	return true;
}

void GUIWindow::Update( ui32 nTimeElapse )
{
	if (!IsVisible())
	{
		return;
	}

	// 判断是否被裁减了，如果被裁减了就不更新
	GUIWindow* pScrollWindow = GetInScroll();
	if (pScrollWindow)
	{
		ERect rtHit = GetHitTestRect();
		if ((rtHit.getWidth() == 0.0f) || (rtHit.getHeight() == 0.0f))
		{
			return;
		}
	}

	UpdateSelf(nTimeElapse);

	updateActionMatrix(nTimeElapse);

// 	if(m_pCurentFlyAnim)
// 	{
// 		m_pCurentFlyAnim->Update(nTimeElapse);
// 	}

	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		m_Children[i]->Update(nTimeElapse);
	}
}

void GUIWindow::UpdateSelf( ui32 nTimeElapse )
{
	if (m_pEffectSystem)
	{
		UpdateEffectArea();

		m_pEffectSystem->_update_sys(nTimeElapse);

		Invalidate();
	}

	if (m_isNeedRenderText && !GUIWindowManager::Instance()->IsEnterBack())
	{
		OnTextChanged();
		m_isNeedRenderText = false;
	}
}

void GUIWindow::SetTouchable( bool bSetting )
{
	if (bSetting != m_bIsTouchable)
	{
		m_bIsTouchable = bSetting;
	}
}

//获得某点对应的可触控窗口
GUIWindow* GUIWindow::GetTargetChildAtPosition( const Vector2& position ) const
{
	const ChildList::const_reverse_iterator end = m_DrawList.rend();

	ChildList::const_reverse_iterator child;
	for (child = m_DrawList.rbegin(); child != end; ++child)
	{
		if ((*child)->IsVisible())
		{
			// recursively scan children of this child windows...			
			GUIWindow* const wnd =
				(*child)->GetTargetChildAtPosition(position);

			// return window pointer if we found a 'hit' down the chain somewhere
			if (wnd)
				return wnd;
			// see if this child is hit and return it's pointer if it is
			else if ((*child)->IsTouchable() && (*child)->IsHit(position))
				return (*child);
		}
	}

	// nothing hit
	return 0;
}

bool GUIWindow::IsScrollWindow(GUIWindowType eType)
{
	if (eType == GWT_LIST ||
		eType == GWT_LIST_HORIZONTAL ||
		eType == GWT_TABLE_VIEW ||
		eType == GWT_GRID_VIEW ||
		eType == GWT_PAGE_VIEW ||
		eType == GWT_SCROLLABLE_PANE ||
		eType == GWT_SCROLL_CARD
		)
	{
		return true;
	}

	return false;
}

// void GUIWindow::BeginRT()
// {
// 	// 判断是否是rendertarge
// 	if (m_bIsRenderTarget && m_nRenderTargetID > 0)
// 	{
// 		// 先把之前的缓冲清掉
// 		GUIRenderManager::Instance()->DrawBuffer();
// 
// 		// 需要刷新的时候再刷新，一般是当子控件有变化的时候
// 		if (m_bNeedRefreshRT)
// 		{
// 			//// 重新设置投影矩阵
// 			Vector2 pixelSize = GetPixelSize();
// 			GUIRenderTargetManager::Instance()->BeginRenderTarget(m_nRenderTargetID, (uint)PixelAligned(pixelSize.x), (uint)PixelAligned(pixelSize.y));
// 		}
// 	}
// }

// void GUIWindow::EndRT()
// {
// 	if (m_bIsRenderTarget && m_nRenderTargetID > 0)
// 	{
// 		// 渲染结束 把rendertarget考到屏幕上
// 		// 先把缓冲都刷上去
// 		GUIRenderManager::Instance()->DrawBuffer();
// 
// 		if (m_bNeedRefreshRT)
// 		{
// 			GUIRenderTargetManager::Instance()->EndRenderTarget();
// 			m_bNeedRefreshRT = false;
// 		}
// 		
// 		if (m_pAnimateController && !m_pAnimateController->GetNeedRT())
// 		{
// 			return;
// 		}
// 
// 		RenderTarget* pRenderTarget = RenderTargetManager::Instance()->getRenderTargetByID(RenderTargetID(m_nRenderTargetID));
// 
// 		if (pRenderTarget && pRenderTarget->getBindTexture())
// 		{
// 			GUIRenderManager::Instance()->AddQuadToBuffer(GetRTRenderArea(), ERect(0.0f, 1.0f, 1.0f, 0.0f), 
// 				ColorRect(m_drawColor, m_drawColor, m_drawColor, m_drawColor), pRenderTarget->getBindTexture(), false, m_fRotate, m_fRotateY, m_fRotateX);
// 			GUIRenderManager::Instance()->DrawBuffer(true);
// 		}
// 
// 		// 不用end 结束的时候会end
// 	}
// }

void GUIWindow::DrawBackground()
{
	if (m_backgroundColor.a == 0)
	{
		return;
	}
	auto texture = GUIRenderManager::Instance()->GetCurrentTexture1();
	auto imageSet = GUIImagesetManager::Instance()->findImageSet(texture);
	if (!imageSet || !imageSet->GetImage("white_pixel"))
	{
		imageSet = m_defaultBackgroundImageset;
	}
	auto image = imageSet->GetImage("white_pixel");
	ImageBufferListType imageBuffers;
	image->DrawImage(imageBuffers, GetRenderArea(), {m_backgroundColor, m_backgroundColor, m_backgroundColor, m_backgroundColor}, m_fRotate, m_fRotateY, m_fRotateX);
	for (const auto& imageBuffer : imageBuffers)
	{
		if (imageBuffer.imageset)
		{
			// 要乘以变幻矩阵
			imageBuffer.imageset->DrawImage(imageBuffer.lt * getActionMatrix(), imageBuffer.rb * getActionMatrix(),
				imageBuffer.lb * getActionMatrix(), imageBuffer.rt * getActionMatrix(), imageBuffer.uv, imageBuffer.color, getMaterial(), getMaskTexture());
		}
	}
}

void GUIWindow::RenderBuffer()
{
	OnBeforeDraw();
	DrawBackground();
	DrawSelf();
	for (GUIWindow* pWindow : m_DrawList)
	{
		pWindow->Render();
	}

	OnEndDraw();
}

//找从当前窗口到根窗口间，类型为GWT_LIST的窗口，没有则返回NULL
GUIWindow* GUIWindow::GetInScroll()
{
	if (!m_pParent)
	{
		return NULL;
	}

	if (IsScrollWindow(m_nType))
	{
		return this;
	}

	if (IsScrollWindow(m_pParent->GetType()))
	{
		return m_pParent;
	}

	return m_pParent->GetInScroll();
}

void GUIWindow::OnParentSized()
{
	// set window area back on itself to cause minimum and maximum size
	// constraints to be applied as required.  (fire no events though)
	SetArea_impl(m_Area.getPosition(), m_Area.getSize(), false);

	const bool moved =
		((m_Area.d_min.d_x.d_scale != 0) || (m_Area.d_min.d_y.d_scale != 0) ||
		(m_HorizontalAlignment != HA_LEFT) || (m_VerticalAlignment != VA_TOP));
	const bool sized =
		((m_Area.d_max.d_x.d_scale != 0) || (m_Area.d_max.d_y.d_scale != 0));

	// now see if events should be fired.
	if (moved)
	{
		OnMoved();
	}

	if (sized)
	{
		OnSized();
	}
}

void GUIWindow::ExportXMLData(xml_document<>& doc,  xml_node<>* pParentNode )
{
	LordAssert(pParentNode);

	// 先输出自己
	xml_node<>* pWindowNode =  doc.allocate_node(node_element,"Window",NULL);
	pParentNode->append_node(pWindowNode);
	GUIString strWindowType = GetType().value;
	char* strType = doc.allocate_string(strWindowType.c_str());
	char* strWindowName = doc.allocate_string(m_strName.c_str());
	xml_attribute<>* pAttribute = doc.allocate_attribute("Type", strType);
	pWindowNode->append_attribute(pAttribute);
	pAttribute = doc.allocate_attribute("Name", strWindowName);
	pWindowNode->append_attribute(pAttribute);

	// bool isHadAnim = false;
	// 属性
	GUIPropertyList propertyList;
	GetPropertyList(propertyList);
	for (size_t i = 0; i < propertyList.size(); ++i)
	{
		GUIString strValue;
		GetProperty(propertyList[i], strValue);

		if (propertyList[i] == GWP_WINDOW_TYPE ||
			propertyList[i] == GWP_WINDOW_NAME)
		{
			continue;
		}

		if (GUIWindowManager::Instance()->m_defalutValue.find(propertyList[i]) != GUIWindowManager::Instance()->m_defalutValue.end() &&
			GUIWindowManager::Instance()->m_defalutValue[propertyList[i]] == strValue)
		{
			continue;
		}
		if (propertyList[i] == GWP_WINDOW_AREA)
		{
			URect tempArea = StringUtil::ParseURect(strValue.c_str());
			URect finalArea = tempArea;

			Vector2 parentSize;
			GUIWindow* pParent = GetParent();
			if (pParent)
			{
				parentSize = pParent->GetPixelSize();
			}
			else
			{
				parentSize = GUIRenderManager::Instance()->GetDisplaySize();
			}

			if (IsAutoSize())
			{
				finalArea.d_min.d_x.d_scale = tempArea.d_min.d_x.asRelative(parentSize.x);
				finalArea.d_min.d_x.d_offset = 0;
				finalArea.d_min.d_y.d_scale = tempArea.d_min.d_y.asRelative(parentSize.y);
				finalArea.d_min.d_y.d_offset = 0;
				finalArea.d_max.d_x.d_scale = tempArea.d_max.d_x.asRelative(parentSize.x);
				finalArea.d_max.d_x.d_offset = 0;
				finalArea.d_max.d_y.d_scale = tempArea.d_max.d_y.asRelative(parentSize.y);
				finalArea.d_max.d_y.d_offset = 0;
			} 
			//else
			//{
			//	finalArea.d_min.d_x.d_scale = tempArea.d_min.d_x.asRelative(parentSize.x);
			//	finalArea.d_min.d_x.d_offset = 0;
			//	finalArea.d_min.d_y.d_scale = tempArea.d_min.d_y.asRelative(parentSize.y);
			//	finalArea.d_min.d_y.d_offset = 0;

			//	UVector2 size;
			//	size.d_x.d_scale = 0;
			//	size.d_x.d_offset = tempArea.getWidth().asAbsolute(parentSize.x);
			//	size.d_y.d_scale = 0;
			//	size.d_y.d_offset = tempArea.getHeight().asAbsolute(parentSize.y);
			//	finalArea.setSize(size);
			//}

			strValue = StringUtil::ToString(finalArea).c_str();
		}

		xml_node<>* pPropertyNode =  doc.allocate_node(node_element,"Property",NULL);
		pWindowNode->append_node(pPropertyNode);
		char* strName = doc.allocate_string(propertyList[i].c_str());
		char* cstrValue = doc.allocate_string(strValue.c_str());
		pAttribute = doc.allocate_attribute("Name", strName);
		pPropertyNode->append_attribute(pAttribute);
		pAttribute = doc.allocate_attribute("Value", cstrValue);
		pPropertyNode->append_attribute(pAttribute);
		// if((String)strName == "FlyAnimCount" &&(String)cstrValue > "0")
		// {
		//		isHadAnim = true;
		// }
	}

// 	if(isHadAnim)
// 	{
// 		GUIFlyAnimateController* m_pLastFlyAnim = m_pCurentFlyAnim;
// 		for(int a = 1 ; a<= m_FlyAnimCount; ++a)
// 		{   
// 			m_pCurentFlyAnim = m_pFlyAnims[a];
// 			propertyList.clear();
// 			GetAnimPropertyList(propertyList);
// 			for (size_t i = 0; i < propertyList.size(); ++i)
// 			{
// 				GUIString strValue;
// 				GetProperty(propertyList[i], strValue);
// 
// 				xml_node<>* pPropertyNode =  doc.allocate_node(node_element,"Property",NULL);
// 				pWindowNode->append_node(pPropertyNode);
// 				char* strName = doc.allocate_string(propertyList[i].c_str());
// 				char* cstrValue = doc.allocate_string(strValue.c_str());
// 				pAttribute = doc.allocate_attribute("Name", strName);
// 				pPropertyNode->append_attribute(pAttribute);
// 				pAttribute = doc.allocate_attribute("Value", cstrValue);
// 				pPropertyNode->append_attribute(pAttribute);
// 			}
// 
// 			xml_node<>* pPropertyNode =  doc.allocate_node(node_element,"Property",NULL);
// 			pWindowNode->append_node(pPropertyNode);
// 			GUIString strValue = StringUtil::ToString(m_pCurentFlyAnim->GetBeginArea()).c_str();
// 			char* cstrValue = doc.allocate_string(strValue.c_str());
// 			pAttribute = doc.allocate_attribute("Name", "FlyBeginArea");
// 			pPropertyNode->append_attribute(pAttribute);
// 			pAttribute = doc.allocate_attribute("Value", cstrValue);
// 			pPropertyNode->append_attribute(pAttribute);
// 
// 			pPropertyNode =  doc.allocate_node(node_element,"Property",NULL);
// 			pWindowNode->append_node(pPropertyNode);
// 			strValue = StringUtil::ToString(m_pCurentFlyAnim->GetEndArea()).c_str();
// 			cstrValue = doc.allocate_string(strValue.c_str());
// 			pAttribute = doc.allocate_attribute("Name", "FlyEndArea");
// 			pPropertyNode->append_attribute(pAttribute);
// 			pAttribute = doc.allocate_attribute("Value", cstrValue);
// 			pPropertyNode->append_attribute(pAttribute);
// 
// 		}
// 		m_pCurentFlyAnim =  m_pLastFlyAnim;
//	}

	// child
	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		m_Children[i]->ExportXMLData(doc, pWindowNode);
	}
}

void GUIWindow::ImportXMLData( xml_node<>* pNode )
{
	// property
	xml_node<>* pChildNode = pNode->first_node();
	while (pChildNode)
	{
		GUIString strNodeName = pChildNode->name();
		if (strNodeName == "Property")
		{
			xml_attribute<>* pAttri = pChildNode->first_attribute();
			GUIString strName = (utf8*)pAttri->value();
			pAttri = pAttri->next_attribute();
			GUIString strValue = (utf8*)pAttri->value();
			SetProperty(strName, strValue);

// 			if (strName == GWP_WINDOW_AREA)
// 			{
// 				m_SavedArea = StringUtil::ParseURect(strValue.c_str());
// 			}
// 			else if (strName == GWP_WINDOW_ROTATEY)
// 			{
// 				m_fSavedRotateY = StringUtil::ParseFloat(strValue.c_str());
// 			}
// 			else if(strName == "FlyBeginArea" && m_pCurentFlyAnim)
// 			{
// 				m_pCurentFlyAnim->SetBeginArea(StringUtil::ParseURect(strValue.c_str()));
// 			}
// 			else if(strName == "FlyEndArea" && m_pCurentFlyAnim)
// 			{
// 				m_pCurentFlyAnim->SetEndArea(StringUtil::ParseURect(strValue.c_str()));
// 			}
		}
		else if (strNodeName == "Window")
		{
			xml_attribute<>* pAttri = pChildNode->first_attribute();
			GUIString strWindowType = pAttri->value();
			pAttri = pAttri->next_attribute();
			GUIString strWindowName = pAttri->value();

			GUIWindow* pWindow = GUIWindowManager::Instance()->CreateGUIWindow(strWindowType, strWindowName);
			LordAssert(pWindow);
			AddChildWindow(pWindow);

			pWindow->ImportXMLData(pChildNode);
		}

		pChildNode = pChildNode->next_sibling();
	}
}

void GUIWindow::ExportJSONData(rapidjson::Document* doc, rapidjson::Value& node)
{
	rapidjson::Value windowType(rapidjson::StringRef(GetType().value));
	node.AddMember("Type", windowType, doc->GetAllocator());

	rapidjson::Value windowName(rapidjson::StringRef(m_strName.c_str()));
	node.AddMember("Name", windowName, doc->GetAllocator());

	//添加属性
	rapidjson::Value propertyArray(rapidjson::kArrayType);
	node.AddMember("Property", propertyArray, doc->GetAllocator());
	GUIPropertyList propertyList;
	GetPropertyList(propertyList);

	for (size_t i = 0; i < propertyList.size(); ++i)
	{

		GUIString strValue;
		GetProperty(propertyList[i], strValue);

		if (propertyList[i] == GWP_WINDOW_TYPE ||
			propertyList[i] == GWP_WINDOW_NAME)
		{
			continue;
		}

		if (GUIWindowManager::Instance()->m_defalutValue.find(propertyList[i]) != GUIWindowManager::Instance()->m_defalutValue.end() &&
			GUIWindowManager::Instance()->m_defalutValue[propertyList[i]] == strValue)
		{
			continue;
		}

		if (propertyList[i] == GWP_WINDOW_AREA)
		{
			URect tempArea = StringUtil::ParseURect(strValue.c_str());
			URect finalArea = tempArea;

			Vector2 parentSize;
			GUIWindow* pParent = GetParent();
			if (pParent)
			{
				parentSize = pParent->GetPixelSize();
			}
			else
			{
				parentSize = GUIRenderManager::Instance()->GetDisplaySize();
			}

			if (IsAutoSize())
			{
				finalArea.d_min.d_x.d_scale = tempArea.d_min.d_x.asRelative(parentSize.x);
				finalArea.d_min.d_x.d_offset = 0;
				finalArea.d_min.d_y.d_scale = tempArea.d_min.d_y.asRelative(parentSize.y);
				finalArea.d_min.d_y.d_offset = 0;
				finalArea.d_max.d_x.d_scale = tempArea.d_max.d_x.asRelative(parentSize.x);
				finalArea.d_max.d_x.d_offset = 0;
				finalArea.d_max.d_y.d_scale = tempArea.d_max.d_y.asRelative(parentSize.y);
				finalArea.d_max.d_y.d_offset = 0;
			}
			//else
			//{
			//	finalArea.d_min.d_x.d_scale = tempArea.d_min.d_x.asRelative(parentSize.x);
			//	finalArea.d_min.d_x.d_offset = 0;
			//	finalArea.d_min.d_y.d_scale = tempArea.d_min.d_y.asRelative(parentSize.y);
			//	finalArea.d_min.d_y.d_offset = 0;

			//	UVector2 size;
			//	size.d_x.d_scale = 0;
			//	size.d_x.d_offset = tempArea.getWidth().asAbsolute(parentSize.x);
			//	size.d_y.d_scale = 0;
			//	size.d_y.d_offset = tempArea.getHeight().asAbsolute(parentSize.y);
			//	finalArea.setSize(size);
			//}

			strValue = StringUtil::ToString(finalArea).c_str();
		}		


		rapidjson::Value propertyObj(rapidjson::kObjectType);		

		rapidjson::Value propertyName;


		char buffer[256];
		int len = sprintf(buffer, "%s", propertyList[i].c_str());				
		propertyName.SetString(buffer, len, doc->GetAllocator());

		len = sprintf(buffer, "%s", strValue.c_str());
		rapidjson::Value propertyValue;		
		propertyValue.SetString(buffer, len, doc->GetAllocator());

		propertyObj.AddMember("Name", propertyName, doc->GetAllocator());
		propertyObj.AddMember("Value", propertyValue, doc->GetAllocator());
		node["Property"].PushBack(propertyObj, doc->GetAllocator());
		
	}



	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
	doc->Accept(writer);

	
	//添加子窗口	
	if (GetChildCount() > 0)
	{
		rapidjson::Value childWindowArray(rapidjson::kArrayType);
		node.AddMember("Window", childWindowArray, doc->GetAllocator());

		for (size_t i = 0; i < GetChildCount(); ++i)
		{
			rapidjson::Value childWindowObj(rapidjson::kObjectType);
			node["Window"].PushBack(childWindowObj, doc->GetAllocator());
			m_Children[i]->ExportJSONData(doc, node["Window"][i]);
		}
	}
	
	

	
}

void GUIWindow::ImportJSONData(const rapidjson::Value& node)
{
	

	rapidjson::Value::ConstMemberIterator property = node.FindMember("Property");
	rapidjson::Value::ConstMemberIterator window = node.FindMember("Window");
		
	
	
	if(property != node.MemberEnd())
	{
		LordAssert(property->value.IsArray());
		//遍历Property数组
		for (rapidjson::Value::ConstValueIterator itProperty = property->value.Begin(); itProperty != property->value.End(); ++itProperty)
		{			
			GUIString strName = (*itProperty)["Name"].GetString();
			GUIString strValue = (*itProperty)["Value"].GetString();
			SetProperty(strName, strValue);
		}
	}
			
	if(window != node.MemberEnd())
	{
		LordAssert(window->value.IsArray());
		//遍历window数组
		for (rapidjson::Value::ConstValueIterator itWindow = window->value.Begin(); itWindow != window->value.End(); ++itWindow)
		{		
			GUIString strWindowType = (*itWindow)["Type"].GetString();
			GUIString strWindowName = (*itWindow)["Name"].GetString();
			GUIWindow* pWindow = GUIWindowManager::Instance()->CreateGUIWindow(strWindowType, strWindowName);
			LordAssert(pWindow);
			AddChildWindow(pWindow);
			pWindow->ImportJSONData(*itWindow);
		}
		
	}
	
}

void GUIWindow::ImportTemplateData(const GUIString& strNamePreffix, xml_node<>* pNode )
{
	// property
	xml_node<>* pChildNode = pNode->first_node();
	while (pChildNode)
	{
		GUIString strNodeName = pChildNode->name();
		if (strNodeName == "Property")
		{
			xml_attribute<>* pAttri = pChildNode->first_attribute();
			GUIString strName = (utf8*)pAttri->value();
			pAttri = pAttri->next_attribute();
			GUIString strValue = (utf8*)pAttri->value();

			SetProperty(strName, strValue);

// 			if (strName == GWP_WINDOW_AREA)
// 			{
// 				m_SavedArea = StringUtil::ParseURect(strValue.c_str());
// 			}
// 			else if (strName == GWP_WINDOW_ROTATEY)
// 			{
// 				m_fSavedRotateY = StringUtil::ParseFloat(strValue.c_str());
// 			}
		}
		else if (strNodeName == "Window")
		{
			xml_attribute<>* pAttri = pChildNode->first_attribute();
			GUIString strWindowType = pAttri->value();
			pAttri = pAttri->next_attribute();
			GUIString strWindowName = pAttri->value();

			GUIWindow* pWindow = GUIWindowManager::Instance()->CreateGUIWindow(strWindowType, strNamePreffix+"_"+strWindowName);
			LordAssert(pWindow);
			AddChildWindow(pWindow);

			pWindow->ImportTemplateData(strNamePreffix, pChildNode);
		}

		pChildNode = pChildNode->next_sibling();
	}
}

const UVector2& GUIWindow::GetPosition() const
{
	return m_Area.d_min;
}

const UDim& GUIWindow::GetXPosition() const
{
	return m_Area.d_min.d_x;
}

const UDim& GUIWindow::GetYPosition() const
{
	return m_Area.d_min.d_y;
}

void GUIWindow::ShowOnTop()
{
	if (!m_pParent)
	{
		return;
	}

	size_t nCount = m_pParent->GetChildCount();
	for (size_t i = 0; i < nCount - 1; ++i)
	{
		if (m_pParent->m_DrawList[i] == this)
		{
			std::swap(m_pParent->m_DrawList[i], m_pParent->m_DrawList[i + 1]);
		}
	}
}

void GUIWindow::MoveFront()
{
	if (!m_pParent)
	{
		return;
	}

	size_t nCount = m_pParent->GetChildCount();
	for (size_t i = 0; i < nCount; ++i)
	{
		GUIWindow* pWindow = m_pParent->m_DrawList[i];
		if (pWindow == this && i != (nCount-1))
		{
			GUIWindow* temp = m_pParent->m_DrawList[i];
			m_pParent->m_DrawList[i] = m_pParent->m_DrawList[i+1];
			m_pParent->m_DrawList[i+1] = temp;
			break;
		}
	}

	for (size_t i = 0; i < nCount; ++i)
	{
		GUIWindow* pWindow = m_pParent->m_Children[i];
		if (pWindow == this && i != (nCount-1))
		{
			GUIWindow* temp = m_pParent->m_Children[i];
			m_pParent->m_Children[i] = m_pParent->m_Children[i+1];
			m_pParent->m_Children[i+1] = temp;
			break;
		}
	}
}

void GUIWindow::MoveBack()
{
	if (!m_pParent)
	{
		return;
	}

	size_t nCount = m_pParent->GetChildCount();
	for (size_t i = 0; i < nCount; ++i)
	{
		GUIWindow* pWindow = m_pParent->m_DrawList[i];
		if (pWindow == this && i != 0)
		{
			GUIWindow* temp = m_pParent->m_DrawList[i];
			m_pParent->m_DrawList[i] = m_pParent->m_DrawList[i-1];
			m_pParent->m_DrawList[i-1] = temp;
			break;
		}
	}

	for (size_t i = 0; i < nCount; ++i)
	{
		GUIWindow* pWindow = m_pParent->m_Children[i];
		if (pWindow == this && i != 0)
		{
			GUIWindow* temp = m_pParent->m_Children[i];
			m_pParent->m_Children[i] = m_pParent->m_Children[i-1];
			m_pParent->m_Children[i-1] = temp;
			break;
		}
	}
}

LORD::UDim GUIWindow::GetWidth() const
{
	return m_Area.getSize().d_x;
}

LORD::UDim GUIWindow::GetHeight() const
{
	return m_Area.getSize().d_y;
}

GUIString GUIWindow::GetOwnerDialogName()
{
	if (m_bIsDialog)
	{
		return m_strName;
	}
	else if(m_pParent)
	{
		return m_pParent->GetOwnerDialogName();
	}
	else
	{
		return "";
	}
}

void GUIWindow::Rename( const GUIString& strNewName )
{
	if (GUIWindowManager::Instance()->IsGUIWindowPresent(m_strName))
	{
		GUIWindowManager::Instance()->RenameWindow(this, strNewName);
		return;
	}

	if (GUIWindowManager::Instance()->IsGUIWindowPresent(strNewName))
	{
		LordLogFatal("control has this name %s is exist!!", strNewName.c_str());
		LordAssert(0);
		return;
	}

	m_strName = strNewName;
}

// void GUIWindow::SetRenderTarget(bool bSetting)
// {
// 	if (bSetting != m_bIsRenderTarget)
// 	{
// 		m_bIsRenderTarget = bSetting;
// 
// 		if (m_bIsRenderTarget)
// 		{
// 			// 创建新的rendertarget
// 			LordAssert(m_nRenderTargetID < 0);
// 			Vector2 vSize = GetPixelSize();
// 			m_nRenderTargetID = GUIRenderTargetManager::Instance()->CreateRenderTarget(vSize.x, vSize.y);
// 			InvalidateRT();
// 		} 
// 		else
// 		{
// 			// 释放掉rendertarget
// 			if (m_nRenderTargetID > 0)
// 			{
// 				GUIRenderTargetManager::Instance()->DestroyRenderTarget(m_nRenderTargetID);
// 				m_nRenderTargetID = -1;
// 			}
// 		}
// 	}
// }

// void GUIWindow::GetRenderCtxOffset(Vector2& offset)
// {
// 	if (m_bIsRenderTarget)
// 	{
// 		offset.x = GetUnclippedOuterRect().left;
// 		offset.y = GetUnclippedOuterRect().top;
// 	}
// 	else if (m_pParent)
// 	{
// 		m_pParent->GetRenderCtxOffset(offset);
// 	}
// 	else
// 	{
// 		offset = Vector2::ZERO;
// 	}
// }

// void GUIWindow::InvalidateRT()
// {
// 	if (m_bIsRenderTarget)
// 	{
// 		if (m_pAnimateController && m_pAnimateController->IsStarted())
// 		{
// 		}
// 		else
// 		{
// 			m_bNeedRefreshRT = true;
// 		}
// 	}
// 	else if (m_pParent)
// 	{
// 		m_pParent->InvalidateRT();
// 	}
// 	else
// 	{
// 
// 	}
// }

void GUIWindow::SetLevel( int nLevel )
{
	if (nLevel != m_nLevel)
	{
		m_nLevel = nLevel;
		OnLevelChange();
	}
}

void GUIWindow::SetAlwaysOnTop( bool bSetting )
{
	if (bSetting != m_bAlwaysOnTop)
	{
		m_bAlwaysOnTop = bSetting;
		OnLevelChange();
	}
}

void GUIWindow::SetBackgroundColor(const Color & color)
{
	if (color != m_backgroundColor)
	{
		m_backgroundColor = color;
	}
}

void GUIWindow::OnLevelChange()
{
	if (m_pParent)
	{
		GUIWindow* const org_parent = m_pParent;
		org_parent->RemoveChild_impl(this);
		org_parent->AddChild_impl(this);
	}
}

void GUIWindow::SetAlpha( float fAlpha )
{
	if (fAlpha != m_fAlpha )
	{
		m_fAlpha = fAlpha;
		
		m_fAlpha = Math::Clamp(m_fAlpha, 0.0f, 1.0f);

		unsigned char alpha = (unsigned char)(255 * m_fAlpha);

		//m_drawColor = (m_drawColor & 0x00FFFFFF) | (alpha << 24);
		m_TextOriColor = (m_TextOriColor & 0x00FFFFFF) | (alpha << 24);

		const size_t child_count = GetChildCount();

		for (size_t i = 0; i < child_count; ++i)
		{
			m_Children[i]->SetAlpha(fAlpha);
		}
	}
}

// void GUIWindow::SetAnimateType( GUIAnimateController::AnimateType nType )
// {
// 	if (m_pAnimateController)
// 	{
// 		m_pAnimateController->SetAnimateType(nType);
// 	}
// }
// 
// void GUIWindow::StartAnimate()
// {
// 	if (m_pAnimateController)
// 	{
// 		m_pAnimateController->Start();
// 	}
// }

void GUIWindow::ChangeAllChildToReletive()
{
	ToReletive();

	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		m_Children[i]->ChangeAllChildToReletive();
	}
}

void GUIWindow::ChangeAllChildToAbsolute()
{
	ToAbsolute();

	for (size_t i = 0; i < GetChildCount(); ++i)
	{
		m_Children[i]->ChangeAllChildToAbsolute();
	}
}

void GUIWindow::ToReletive()
{
	Vector2 parentSize;
	GUIWindow* pParent = GetParent();
	if (pParent)
	{
		parentSize = pParent->GetPixelSize();
	}
	else
	{
		parentSize = GUIRenderManager::Instance()->GetDisplaySize();
	}

	URect newArea;
	newArea.d_min.d_x.d_scale = m_Area.d_min.d_x.asRelative(parentSize.x);
	newArea.d_min.d_x.d_offset = 0;
	newArea.d_min.d_y.d_scale = m_Area.d_min.d_y.asRelative(parentSize.y);
	newArea.d_min.d_y.d_offset = 0;
	newArea.d_max.d_x.d_scale = m_Area.d_max.d_x.asRelative(parentSize.x);
	newArea.d_max.d_x.d_offset = 0;
	newArea.d_max.d_y.d_scale = m_Area.d_max.d_y.asRelative(parentSize.y);
	newArea.d_max.d_y.d_offset = 0;

	SetArea(newArea);
}

void GUIWindow::ToAbsolute()
{
	Vector2 parentSize;
	GUIWindow* pParent = GetParent();
	if (pParent)
	{
		parentSize = pParent->GetPixelSize();
	}
	else
	{
		parentSize = GUIRenderManager::Instance()->GetDisplaySize();
	}

	URect newArea;
	newArea.d_min.d_x.d_offset = m_Area.d_min.d_x.asAbsolute(parentSize.x);
	newArea.d_min.d_x.d_scale = 0;
	newArea.d_min.d_y.d_offset = m_Area.d_min.d_y.asAbsolute(parentSize.y);
	newArea.d_min.d_y.d_scale = 0;
	newArea.d_max.d_x.d_offset = m_Area.d_max.d_x.asAbsolute(parentSize.x);
	newArea.d_max.d_x.d_scale = 0;
	newArea.d_max.d_y.d_offset = m_Area.d_max.d_y.asAbsolute(parentSize.y);
	newArea.d_max.d_y.d_scale = 0;

	SetArea(newArea);
}

void GUIWindow::SetScale(const Vector3& scale)
{
	SetActionMatrixScale(scale);
}

void GUIWindow::SetRotate( float fRotate )
{
	if (fRotate != m_fRotate)
	{
		m_fRotate = fRotate;
	}
}

void GUIWindow::SetRotateY(float fRotate)
{
	if (fRotate != m_fRotateY)
	{
		m_fRotateY = fRotate;
	}
}

void GUIWindow::SetRotateX(float fRotate)
{
	if (fRotate != m_fRotateX)
	{
		m_fRotateX = fRotate;
	}
}

void GUIWindow::PlaySoundForWndOpen()
{
	SoundSystem::Instance()->playEffectByType((SoundType)m_nwndOpenSoundName);
}

void GUIWindow::PlaySoundForWndClose()
{
	SoundSystem::Instance()->playEffectByType((SoundType)m_nwndCloseSoundName);
}
// void GUIWindow::GetAnimPropertyList( GUIPropertyList& properlist )
// {
// 	properlist.push_back(GWP_WINDOW_FLY_ANIM_SELECT);
// 	if(m_pCurentFlyAnim)
// 	{
// 		properlist.push_back(GWP_WINDOW_FLY_ANIMATE_TYPE);
// 		properlist.push_back(GWP_WINDOW_FLY_ANIMATE_ACCELERATION);
// 		properlist.push_back(GWP_WINDOW_FLY_ANIMATE_ACC_DIR);
// 		properlist.push_back(GWP_WINDOW_FLY_ANIMATE_TIME);
// 	}
// 
// }

// void GUIWindow::SetFlyBeginPoint()
// {
// 	if(m_pCurentFlyAnim)
// 	{
// 		m_pCurentFlyAnim->SetBeginArea();
// 	}
// }

// void GUIWindow::SetFlyEndPoint()
// {
// 	if(m_pCurentFlyAnim)
// 	{
// 		m_pCurentFlyAnim->SetEndArea();
// 	}
// }

// void GUIWindow::SetFlyAnimCount( int Setting )
// {
// 	if(Setting < 0)
// 	{
// 		return;
// 	}
// 	else if(Setting > m_FlyAnimCount)
// 	{
// 		for(int i = 1;i<= Setting - m_FlyAnimCount;++i)
// 		{	
// 			GUIFlyAnimateController* __pFlyAnim = LordNew GUIFlyAnimateController(this);
// 			m_pFlyAnims.insert(std::pair<int, GUIFlyAnimateController*>(m_FlyAnimCount + i, __pFlyAnim));
// 		}
// 	}
// 	else if(Setting < m_FlyAnimCount)
// 	{
// 		for(int i = m_FlyAnimCount;i> Setting;--i)
// 		{	
// 			FlyAnimMap::iterator it = m_pFlyAnims.find(i);
// 			if(it != m_pFlyAnims.end())
// 			{
// 				GUIFlyAnimateController* __pFlyAnim = it->second;
// 				LordSafeDelete(__pFlyAnim);
// 				m_pFlyAnims.erase(it);
// 			}
// 		}
// 	}
// 
// 	m_FlyAnimCount = Setting;
// }

// void GUIWindow::SetSelectFlyAnim(int index)
// {
// 	m_pCurentFlyAnim = m_pFlyAnims[index];
// }

// int GUIWindow::GetSelectFlyAnim()
// {
// 	FlyAnimMap::iterator it = m_pFlyAnims.begin();
// 	for(;it != m_pFlyAnims.end(); ++it)
// 	{
// 		if(m_pCurentFlyAnim == it->second)
// 			return it->first;
// 	}
// 	m_pCurentFlyAnim = NULL;
// 	return 0;
// }

// void GUIWindow::PlayFLyAnim( const String& name )
// {
// 	StringArray stra = StringUtil::Split(name, "_");
// 	int _index = StringUtil::ParseInt(stra.at(1));
// 	FlyAnimMap::iterator it = m_pFlyAnims.find(_index);
// 	if(it != m_pFlyAnims.end())
// 	{
// 		m_pCurentFlyAnim = it->second;
// 		m_pCurentFlyAnim->Start();
// 	}
// 	else
// 	{
// 		m_pCurentFlyAnim = NULL;
// 	}
// }
// 
// void GUIWindow::StarFLyAnim()
// {
// 	if(m_pCurentFlyAnim)
// 	{
// 		m_pCurentFlyAnim->Start();
// 	}
// }


float GUIWindow::SetEffectName(const GUIString& strEffectName)
{
	//if (m_strEffectName != strEffectName)
	{
		m_strEffectName = strEffectName;

		return PlayEffect();
	}
}

float GUIWindow::PlayEffect()
{
	if (m_pEffectSystem)
	{
		UnprepareEffect();
	}

	if (!m_strEffectName.empty())
	{
		return PrepareEffect();
	}

	return 0.0f;
}

float GUIWindow::PlayEffect(const GUIString& strEffectName)
{
	m_strEffectName = strEffectName;

	return PlayEffect();
}

float GUIWindow::PrepareEffect()
{
	String strName = StringUtil::Format("GUIEffect_%s", m_strName.c_str());

	m_pEffectSystem = EffectSystemManager::Instance()->createEffectSystem(strName, m_strEffectName.c_str(), true);
	LordAssert(m_pEffectSystem);

	m_pParentNode = SceneManager::Instance()->getRootNode();
	m_pSceneNode = m_pParentNode->createChild();
	LordAssert(m_pSceneNode);
	m_pEffectSystem->setParentSceneNode(m_pSceneNode);
	m_pEffectSystem->setCurrentCamera(SceneManager::Instance()->getGUICamera());
	m_pEffectSystem->prepare_sys(true);
	m_pEffectSystem->start();

	m_pSceneNode->setLocalScaling(Vector3::ONE);
	m_pSceneNode->update();
	const Box& box = m_pEffectSystem->getBoundingBox();
	m_fInverseXScale = 1.0f / box.getDX();
	m_fInverseYScale = 1.0f / box.getDY();

	// ----update scene node
	UpdateEffectArea();

	return m_pEffectSystem->getEffectTimeLength();
}

void GUIWindow::UnprepareEffect()
{
	if (m_pEffectSystem)
	{
		m_pEffectSystem->stop();
		m_pEffectSystem->unprepare_sys();
		EffectSystemManager::Instance()->destroyEffectSystem(m_pEffectSystem);
		m_pParentNode->destroyChild(m_pSceneNode);
		m_pSceneNode = NULL;
		m_pParentNode = NULL;
		m_pEffectSystem = NULL;
	}
}

void GUIWindow::UpdateEffectArea()
{
	if (m_pSceneNode)
	{
		// ----update scene node
		// position
		Vector3 position;
		ERect rtOuterRect = GetRenderArea();
		position.x = rtOuterRect.left + rtOuterRect.getWidth() * 0.5f;
		position.y = rtOuterRect.bottom;
		// Camera* pCamera = SceneManager::Instance()->getGUICamera();
		position.z = GUIRenderManager::Instance()->GetDepth();
		m_pSceneNode->setWorldPosition(position);

		// orientation
		Quaternion qx(Vector3::UNIT_X, Math::PI);
		Quaternion qz(Vector3::UNIT_Z, -Math::DEG2RAD*m_fEffectRotate);

		Quaternion q = qx * qz;
		m_pSceneNode->setWorldOrientation(q);

		// size
		m_pSceneNode->setLocalScaling(Vector3(/*m_fInverseXScale**/rtOuterRect.getWidth(), /*m_fInverseYScale* */rtOuterRect.getHeight(), 1.0f));
		m_pSceneNode->update();
	}
}

void GUIWindow::OnEndDraw()
{
	if (m_pEffectSystem)
	{
		GUIRenderManager::Instance()->SetDepth(GUIRenderManager::Instance()->GetDepth() - GetPixelSize().x);
		UpdateEffectArea();

		GUIRenderManager::Instance()->EndRender();
		m_pEffectSystem->_updateRenderQueue();

		// draw effect and actor
		ui8 nStartIndex = SceneManager::Instance()->getRenderQueueIndex("EffectSpecial");
		ui8 nEndIndex = SceneManager::Instance()->getRenderQueueIndex("Effect");

		for (ui32 i = nStartIndex; i <= nEndIndex; ++i)
		{
			RenderQueue* renderQueue = SceneManager::Instance()->getRenderQueueByIndex(i);
			LordAssert(renderQueue);
			renderQueue->renderQueue(true);
			renderQueue->beginRender();
		}
		/*EffectSystemManager::Instance()->endRender(true);
		EffectSystemManager::Instance()->beginRender();*/
		GUIRenderManager::Instance()->BeginRender();
	}
}

// bool GUIWindow::OnTouchDrag(const Vector2& position)
// {
// 	MouseEventArgs args(this);
// 	args.position = position;
// 	fireEvent(EventWindowLongTouchDown, args);
// 
// 	return true;
// }

void GUIWindow::Clone(const GUIString& strNamePreffix, GUIWindow* pWindow)
{
	// 复制属性
	GUIPropertyList propertyList;
	pWindow->GetPropertyList(propertyList);
	GUIPropertyList::iterator itor = propertyList.begin();
	for (; itor != propertyList.end(); ++itor)
	{
		if ((*itor) == GWP_WINDOW_NAME || (*itor) == GWP_WINDOW_TYPE)
			continue;
		GUIString strValue;
		pWindow->GetProperty(*itor, strValue);
		this->SetProperty((*itor), strValue);
	}

	const auto& size = pWindow->GetPixelSize();
	SetSize({ { 0, size.x },{ 0, size.y } });

	// 复制子控件
	int nChildCount = (int)pWindow->GetChildCount();
	for (int i = 0; i < nChildCount; ++i)
	{
		GUIWindow* pCloneWindow = pWindow->GetChildByIndex(i);

		GUIWindow* pNewWindow = GUIWindowManager::Instance()->CreateGUIWindow(pCloneWindow->GetType(), strNamePreffix + pCloneWindow->GetName());
		LordAssert(pNewWindow);
		AddChildWindow(pNewWindow);

		pNewWindow->Clone(strNamePreffix, pCloneWindow);
	}
}

void GUIWindow::updateActionMatrix(ui32 dt)
{
	if (m_isPlayingAction && m_isActionRoot && m_action)
	{
		// 如果当前就是action root，需要更新一下keyframe
		if (m_action->update(dt))
		{
			// 删除action
			endAction();
			
			//由于存在结束不reset的情况
			//return;
		} 
	}

	if (m_isPlayingAction && getActionRoot() && getActionRoot()->getAction())
	{
		ERect actionRootArea = getActionRoot()->GetRenderArea();

		GUIAction* action = getActionRoot()->getAction();
		const GUIActionKeyFrame& actionKeyFrame = action->getCurrentFrameData();

		// alpha
		if (getActionRoot() == this)
		{
			SetAlpha(actionKeyFrame.m_alpha);
		}

		// transform
		float halfWidth = actionRootArea.getWidth() / 2;
		float halfHeight = actionRootArea.getHeight() / 2;

		Vector3 rootOffset(Vector3::ZERO);
		rootOffset.x = -actionRootArea.left - halfWidth;
		rootOffset.y = -actionRootArea.top - halfHeight;
		
		m_actionMatrix.identity();
		// 回归原点
		m_actionMatrix.translate(rootOffset);

		// 以下是变幻的部分
		// 旋转缩放
		Quaternion qx(Vector3::UNIT_X, actionKeyFrame.m_rotate.x);
		Quaternion qy(Vector3::UNIT_Y, actionKeyFrame.m_rotate.y);
		Quaternion qz(Vector3::UNIT_Z, actionKeyFrame.m_rotate.z);

		Quaternion q = qx * qy * qz;

		Matrix4 transformMatrix;
		q.toMat4(transformMatrix);
		transformMatrix.scale(Vector3(actionKeyFrame.m_scale.x, actionKeyFrame.m_scale.y, actionKeyFrame.m_scale.z));
		// 平移
		transformMatrix.translate(Vector3(actionKeyFrame.m_translate.x, actionKeyFrame.m_translate.y, actionKeyFrame.m_translate.z));
		// 变幻的部分结束


		// 回到世界坐标
		Matrix4 translate;
		translate.makeTranslation(Vector3(-rootOffset.x, -rootOffset.y, 0.0f));
		m_actionMatrix = m_actionMatrix * transformMatrix * translate;
	}
}

void GUIWindow::playAction(GUIAction* action, bool endReset)
{
	LordSafeDelete(m_action);

	m_actionEndReset = endReset;

	m_action = action;

	m_isActionRoot = true;

	setChildPlayAction(true);
}

void GUIWindow::setChildPlayAction(bool bPlayed)
{
	if (!bPlayed)
	{
		m_actionMatrix.identity();
	}

	m_isPlayingAction = bPlayed;

	size_t count = GetChildCount();
	for (size_t i = 0; i < count; ++i)
	{
		GUIWindow* child = GetChildByIndex(i);
		if (child)
		{
			child->setChildPlayAction(bPlayed);
		}
	}
}

void GUIWindow::endAction()
{
	if (m_actionEndReset)
	{
		if (m_action)
		{
			const GUIActionKeyFrame& actionKeyFrame	= m_action->getCurrentFrameData();
			SetAlpha(actionKeyFrame.m_alpha);
		}
		LordSafeDelete(m_action);
		m_isActionRoot = false;
		setChildPlayAction(false);

		WindowEventArgs args(this);
		fireEvent(EventWindowActionEnd, args);
	}
}

void GUIWindow::getClippedRect(ERect& rect) const
{
	if (GetParent())
	{
		if (GetParent()->IsClipChild())
		{
			rect = rect.intersect(GetParent()->GetUnclippedOuterRect());
		} 
		else
		{
			GetParent()->getClippedRect(rect);
		}
	}
}

void GUIWindow::setMaskTextureName(const GUIString& textureName)
{
	if (textureName != m_maskTextureName)
	{
		if (m_maskTexture)
		{
			// 如果原来有mask，要释放掉
			TextureManager::Instance()->releaseResource(m_maskTexture);
			m_maskTexture = NULL;
		} 

		m_maskTextureName = textureName;

		if (!m_maskTextureName.empty())
		{
			SamplerState* pState = GUIRenderManager::Instance()->GetMaterial(GUIMaterialType_MASK)->getSamplerStateByTexStage(1);
			m_maskTexture = TextureManager::Instance()->createTexture(m_maskTextureName.c_str());
			m_maskTexture->load();
            m_maskTexture->setSamplerState(pState);
		}
	}
}

void GUIWindow::SetActionMatrixScale(const Vector3& scale, bool root, const Matrix4* matrix)
{
	if (root)
	{
		ERect actionRootArea = GetRenderArea();

		// transform
		float halfWidth = actionRootArea.getWidth() / 2;
		float halfHeight = actionRootArea.getHeight() / 2;

		Vector3 rootOffset(Vector3::ZERO);
		rootOffset.x = -actionRootArea.left - halfWidth;
		rootOffset.y = -actionRootArea.top - halfHeight;

		m_actionMatrix.identity();
		// 回归原点
		m_actionMatrix.translate(rootOffset);

		// 以下是变幻的部分

		Matrix4 transformMatrix;
		transformMatrix.identity();
		transformMatrix.scale(Vector3(scale.x, scale.y, scale.z));

		// 变幻的部分结束


		// 回到世界坐标
		Matrix4 translate;
		translate.makeTranslation(Vector3(-rootOffset.x, -rootOffset.y, 0.0f));
		m_actionMatrix = m_actionMatrix * transformMatrix * translate;

		size_t childCount = GetChildCount();
		for (size_t i = 0; i < childCount; ++i)
		{
			GUIWindow* window = GetChildByIndex(i);
			window->SetActionMatrixScale(scale, false, &m_actionMatrix);
		}
	} 
	else
	{
		LordAssert(matrix);
		m_actionMatrix = (*matrix);

		size_t childCount = GetChildCount();
		for (size_t i = 0; i < childCount; ++i)
		{
			GUIWindow* window = GetChildByIndex(i);
			window->SetActionMatrixScale(scale, false, &m_actionMatrix);
		}
	}
}

bool GUIWindow::onDragging(const Touch* touch)
{
	if (m_dragging == false)
	{
		m_dragging = true;

		TouchEventArgs args(this);
		args.touch = touch;
		fireEvent(EventWindowDragStart, args);
	}

	TouchEventArgs args(this);
	args.touch = touch;

	fireEvent(EventWindowDragging, args);
	if (m_pParent)
	{
		m_pParent->onDragging(touch);
	}
	return true;
}

bool GUIWindow::onDragEnd(const Touch* touch)
{
	TouchEventArgs args(this);
	args.touch = touch;
	fireEvent(EventWindowDragEnd, args);

	m_dragging = false;
	if (m_pParent)
	{
		m_pParent->onDragEnd(touch);
	}
	return true;
}

bool GUIWindow::onLongTouchStart(const Touch* touch)
{
	TouchEventArgs args(this);
	args.touch = touch;
	fireEvent(EventWindowLongTouchStart, args);
	if (m_pParent)
	{
		m_pParent->onLongTouchStart(touch);
	}
	return true;
}

bool GUIWindow::onLongTouchEnd(const Touch* touch)
{
	TouchEventArgs args(this);
	args.touch = touch;
	fireEvent(EventWindowLongTouchEnd, args);
	if (m_pParent)
	{
		m_pParent->onLongTouchEnd(touch);
	}
	return true;
}

void GUIWindow::setGray(bool setting)
{
	GUIMaterialType material = getMaterial();
	if (!setting)
	{
		// 如果是enable状态，并且当前材质是disable的，就转成enable
		if (material >= GUIMaterialType_CULL_NONE_GRAY)
		{
			material = (GUIMaterialType)(material - GUIMaterialType_CULL_NONE_GRAY);
		}
	}
	else
	{
		if (material < GUIMaterialType_CULL_NONE_GRAY)
		{
			material = (GUIMaterialType)(material + GUIMaterialType_CULL_NONE_GRAY);
		}
	}

	setMaterial(material);
}

void GUIWindow::onChildVisibleChange()
{
	// 增加星级显示的居中处理
	size_t nChildCount = GetChildCount();

	if (m_isMiddleLayoutChild && nChildCount > 0 )
	{

		UDim xMin = GetXPosition();
		UDim xMax = xMin + GetWidth();

		UDim xChildAllWidth = UDim(0, 0);

		for (size_t i = 0; i < nChildCount; ++i)
		{
			if (m_DrawList[i]->IsVisible())
			{
				xChildAllWidth = xChildAllWidth + m_DrawList[i]->GetWidth();
			}
		}

		UDim offset = ((xMax - xMin) - xChildAllWidth) * UDim(0.5, 0.5);

		for (size_t i = 0; i < nChildCount; ++i)
		{
			if (m_DrawList[i]->IsVisible())
			{
				m_DrawList[i]->SetXPosition(offset);

				offset = offset + m_DrawList[i]->GetWidth();
			}
		}
	}
}


}
