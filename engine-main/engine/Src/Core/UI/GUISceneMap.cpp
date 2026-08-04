#include "GUISceneMap.h"
#include "GUIWindowManager.h"
#include "GUIStaticImage.h"
#include "GUIRenderManager.h"
#include "GUISystem.h"

namespace LORD
{

	const int MAX_ICON_WINDOW_COUNT = 100;

	GUISceneMap::GUISceneMap( GUIWindowType nType, const GUIString& strName )
		: GUIWindow(nType, strName)
		, m_fSceneWidth(256.0f)
		, m_fSceneHeight(256.0f)
		, m_fSceneScaleX(1.0f)
		, m_fSceneScaleY(1.0f)
		, m_fUIWidth(256.0f)
		, m_fUIHeight(256.0f)
		, m_fNowCenterX(0.5f)
		, m_fNowCenterY(0.5f)
		, m_pSceneMap(NULL)
		, m_bDragging(false)
		, m_dragPosition(Vector2::ZERO)
	{
		SetClipChild(true);
	}

	GUISceneMap::~GUISceneMap()
	{
	}

	bool GUISceneMap::SetProperty( const GUIString& strName, const GUIString& strValue )
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}

		//if (strName == GWP_SI_IMAGE_NAME)
		//{
		//	SetImage(strValue);
		//	return true;
		//}
		//else if (strName == GWP_SI_STRECHED_IMAGE)
		//{
		//	SetStretched(StringUtil::ParseBool(strValue.c_str()));
		//	return true;
		//}
		//else if (strName == GWP_SI_STRECHED_OFFSET)
		//{
		//	m_fStretchOffset = StringUtil::ParseReal(strValue.c_str());
		//	return true;
		//}

		return false;
	}

	bool GUISceneMap::GetProperty( const GUIString& strName, GUIString& strValue )
	{
		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}

		//if (strName == GWP_SI_IMAGE_NAME)
		//{
		//	strValue = m_RenderImage.GetWholeImageName();
		//	return true;
		//}
		//else if (strName == GWP_SI_STRECHED_IMAGE)
		//{
		//	strValue = StringUtil::ToString(m_bIsStretched).c_str();
		//	return true;
		//}
		//else if (strName == GWP_SI_STRECHED_OFFSET)
		//{
		//	strValue = StringUtil::ToString(m_fStretchOffset).c_str();
		//	return true;
		//}

		return false;
	}

	void GUISceneMap::GetPropertyList( GUIPropertyList& properlist )
	{
		GUIWindow::GetPropertyList(properlist);
		//properlist.push_back(GWP_SI_IMAGE_NAME);
		//properlist.push_back(GWP_SI_STRECHED_IMAGE);
		//properlist.push_back(GWP_SI_STRECHED_OFFSET);
	}

	bool GUISceneMap::GetPropertyType( const GUIString& strName, GUIPropertyType& type )
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}

		//if (strName == GWP_SI_IMAGE_NAME)
		//{
		//	type = GPT_IMAGE;
		//	return true;
		//}
		//else if (strName == GWP_SI_STRECHED_IMAGE)
		//{
		//	type = GPT_BOOL;
		//	return true;
		//}
		//else if (strName == GWP_SI_STRECHED_OFFSET)
		//{
		//	type = GPT_FLOAT;
		//	return true;
		//}

		return false;
	}

	void GUISceneMap::InitSceneMap( const GUIString& strImageName, float fSceneWidth, float fSceneHeight, const Vector2& scenePosition )
	{
		ResetAllWindow();

		m_pSceneMap->SetImage(strImageName);

		m_fSceneWidth = fSceneWidth;
		m_fSceneHeight = fSceneHeight;

		m_fUIWidth = m_fSceneWidth * m_fSceneScaleX;
		m_fUIHeight = m_fSceneHeight * m_fSceneScaleY;
		m_pSceneMap->SetSize(UVector2(UDim(0, m_fUIWidth), UDim(0, m_fUIHeight)));

		SetCenterPostion(scenePosition);
	}

	void GUISceneMap::SetCenterPostion( const Vector2& scenePosition )
	{
		m_fNowCenterX = scenePosition.x / m_fSceneWidth;
		m_fNowCenterY = scenePosition.y / m_fSceneHeight;

		m_fNowCenterX = Math::Clamp(m_fNowCenterX, 0.0f, 1.0f);
		m_fNowCenterY = Math::Clamp(m_fNowCenterY, 0.0f, 1.0f);

		float fUIXPos = m_fNowCenterX*m_fUIWidth;
		float fUIYPos = m_fNowCenterY*m_fUIHeight;
		Vector2 pixelSize = GetPixelSize();
		Vector2 halfPixelSize = pixelSize * 0.5f;

		// 相对于父窗口左上角的位置
		fUIXPos = halfPixelSize.x - fUIXPos;
		fUIYPos = halfPixelSize.y - fUIYPos;

		float fMinX = pixelSize.x - m_fUIWidth;
		float fMinY = pixelSize.y - m_fUIHeight;

		fUIXPos = Math::Clamp(fUIXPos, fMinX, 0.0f);
		fUIYPos = Math::Clamp(fUIYPos, fMinY, 0.0f);

		m_pSceneMap->SetPosition(UVector2(UDim(0.0f, fUIXPos), UDim(0.0f, fUIYPos)));
	}

	bool GUISceneMap::TouchDown( const Vector2& position )
	{
		m_dragPosition = position;

		return true;
	}

	bool GUISceneMap::TouchUp( const Vector2& position )
	{
		if (!m_bDragging)
		{
			ERect mapRect = m_pSceneMap->GetUnclippedOuterRect();

			m_targetPosition.x = m_fSceneWidth * (position.x - mapRect.left) / m_fUIWidth;
			m_targetPosition.y = m_fSceneHeight * (position.y - mapRect.top) / m_fUIHeight;

			//GUISystem::Instance()->GetEventCallback()(GET_SCENE_MAP_CLICK, GetOwnerDialogName(), GetName(), 0, 0);
		}

		m_bDragging = false;
		return true;
	}

	bool GUISceneMap::TouchMove( const Vector2& position )
	{
		m_bDragging = true;
		
		Vector2 delta = position - m_dragPosition;
		Vector2 pixelSize = GetPixelSize();
		float fMinX = pixelSize.x - m_fUIWidth;
		float fMinY = pixelSize.y - m_fUIHeight;

		UVector2 oldPosition = m_pSceneMap->GetPosition();
		Vector2 newPosition;
		newPosition.x = delta.x + oldPosition.d_x.d_offset;
		newPosition.y = delta.y + oldPosition.d_y.d_offset;

		newPosition.x = Math::Clamp(newPosition.x, fMinX, 0.0f);
		newPosition.y = Math::Clamp(newPosition.y, fMinY, 0.0f);

		m_pSceneMap->SetPosition(UVector2(UDim(0.0f, newPosition.x), UDim(0.0f, newPosition.y)));

		m_dragPosition = position;
		return true;
	}

	void GUISceneMap::SetIcon( const Vector2& scenePosition, const GUIString& strIconName, const Vector2& iconSize)
	{
		IconInfo iconInfo;
		iconInfo.position = scenePosition;
		iconInfo.strIconImageName = strIconName;
		iconInfo.iconSize = iconSize;

		m_iconList.push_back(iconInfo);
	}

	void GUISceneMap::ClearAllIcon()
	{
		m_iconList.clear();
	}

	void GUISceneMap::UpdateSelf( ui32 nTimeElapse )
	{
		size_t nIconCount = m_iconList.size();
		nIconCount = nIconCount > MAX_ICON_WINDOW_COUNT ? MAX_ICON_WINDOW_COUNT : nIconCount;

		for (size_t i = 0; i < nIconCount; ++i)
		{
			GUIStaticImage* pIcon = (GUIStaticImage*)m_IconWindowList[i];
			pIcon->SetImage(m_iconList[i].strIconImageName);

			pIcon->SetSize(UVector2(UDim(0.0f, m_iconList[i].iconSize.x), UDim(0.0f, m_iconList[i].iconSize.y)));

			float fXPosition = m_fUIWidth * m_iconList[i].position.x / m_fSceneWidth;
			float fYPosition = m_fUIHeight * m_iconList[i].position.y / m_fSceneHeight;

			fXPosition -= m_iconList[i].iconSize.x * 0.5f;
			fYPosition -= m_iconList[i].iconSize.y * 0.5f;

			pIcon->SetPosition(UVector2(UDim(0.0f, fXPosition), UDim(0.0f, fYPosition)));
		}
	}

	void GUISceneMap::SetScale( float fScaleX, float fScaleY )
	{
		m_fSceneScaleX = fScaleX;
		m_fSceneScaleY = fScaleY;
		m_fUIWidth = m_fSceneWidth * m_fSceneScaleX;
		m_fUIHeight = m_fSceneHeight * m_fSceneScaleY;
		m_pSceneMap->SetSize(UVector2(UDim(0, m_fUIWidth), UDim(0, m_fUIHeight)));

	}

	void GUISceneMap::ResetAllWindow()
	{
		if (m_pSceneMap)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_pSceneMap);
			m_pSceneMap = NULL;
		}

		for (size_t i = 0; i < m_IconWindowList.size(); ++i)
		{
			if (m_IconWindowList[i])
			{
				GUIWindowManager::Instance()->DestroyGUIWindow(m_IconWindowList[i]);
			}
		}

		m_IconWindowList.clear();

		m_pSceneMap = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, GetName()+"_sceneMap");
		AddChildWindow(m_pSceneMap);
		m_pSceneMap->SetTouchable(false);

		for (size_t i = 0; i < MAX_ICON_WINDOW_COUNT; ++i)
		{
			GUIString strName = StringUtil::Format("%s_sceneMap_%d", GetName().c_str(), i).c_str();
			GUIWindow* pWindow = GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, strName);
			m_IconWindowList.push_back(pWindow);

			m_pSceneMap->AddChildWindow(pWindow);
			pWindow->SetTouchable(false);
		}

	}

}
