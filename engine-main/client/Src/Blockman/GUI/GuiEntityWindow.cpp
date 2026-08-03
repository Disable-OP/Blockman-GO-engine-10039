#include "GuiEntityWindow.h"

#include "EntityRenders/EntityRender.h"
#include "EntityRenders/EntityRenderable.h"
#include "EntityRenders/EntityRenderManager.h"
#include "GUI/GuiDef.h"
#include "cWorld/Blockman.h"

#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIRenderManager.h"

namespace BLOCKMAN
{

GuiEntityWindow::GuiEntityWindow(const GUIWindowType& nType, const GUIString& strName)
	: BaseClass(nType, strName)
	, m_player(NULL)
	, m_fScaleFactor(1.0f)
	, m_fRotateX(0.0f)
	, m_fRotateY(0.0f)
	, m_fRotateZ(0.0f)
	, m_fScaleClientSet(1.0f)
	, m_fParmaScale(1.f)
{
}

void GuiEntityWindow::DrawSelf()
{
	if (!m_player)
		return;

	Matrix4 viewprojMat = SceneManager::Instance()->getGUICamera()->getViewProjMatrix();

	GUIRenderManager::Instance()->SetDepth(GUIRenderManager::Instance()->GetDepth() - GetPixelSize().x);
	UpdateEntityArea();
	
	EntityRender* pEntityRender = EntityRenderManager::Instance()->getEntityClassRenderObject(ENTITY_CLASS_PLAYER);
	LordAssert(pEntityRender);

	pEntityRender->doRender_gui((Entity*)m_player, m_worldMat, viewprojMat);

	GUIRenderManager::Instance()->BeginRender();
}

void GuiEntityWindow::UpdateSelf(ui32 nTimeElapse)
{
	// todo.
}

void GuiEntityWindow::UpdateEntityArea()
{
	if (!m_player)
		return;

	m_worldMat.identity();

	// orientation
	Quaternion qx(Vector3::UNIT_X, Math::PI + Math::DEG2RAD * m_fRotateX);
	Quaternion qy(Vector3::UNIT_Y, Math::DEG2RAD * m_fRotateY);
	Quaternion qz(Vector3::UNIT_Z, Math::DEG2RAD * m_fRotateZ);
	Quaternion q = qx * qy * qz;
	m_worldMat.fromQuan(q);

	Vector3 position;
	ERect rtOuterRect = GetRenderArea();
	position.x = rtOuterRect.left + rtOuterRect.getWidth() * 0.5f;
	position.y = rtOuterRect.bottom;
	position.z = GUIRenderManager::Instance()->GetDepth();
	m_worldMat.translate(position);

	// size
	float fScale = m_fScaleClientSet * rtOuterRect.getWidth();
	m_worldMat.scale(Vector3(fScale * m_fParmaScale, fScale * m_fParmaScale, fScale * m_fParmaScale));
}

void GuiEntityWindow::OnBeforeDraw()
{
	GUIRenderManager::Instance()->EndRender();
}

void GuiEntityWindow::OnEndDraw()
{
	GUIRenderManager::Instance()->BeginRender();
}

bool GuiEntityWindow::SetProperty(const GUIString& strName, const GUIString& strValue)
{
	if (BaseClass::SetProperty(strName, strValue))
	{
		return true;
	}

	if (strName == GWP_ENTITY_WINDOW_PITCH)
	{
		m_fRotateX = StringUtil::ParseFloat(strValue.c_str());
		return true;
	}
	else if (strName == GWP_ENTITY_WINDOW_YAW)
	{
		m_fRotateY = StringUtil::ParseFloat(strValue.c_str());
		return true;
	}
	else if (strName == GWP_ENTITY_WINDOW_ROLL)
	{
		m_fRotateZ = StringUtil::ParseFloat(strValue.c_str());
		return true;
	}
	else if (strName == GWP_ENTITY_WINDOW_SCALE)
	{
		m_fScaleClientSet = StringUtil::ParseFloat(strValue.c_str());
		return true;
	}

	return false;
}

bool GuiEntityWindow::GetProperty(const GUIString& strName, GUIString& strValue)
{
	if (BaseClass::GetProperty(strName, strValue))
	{
		return true;
	}

	if (strName == GWP_ENTITY_WINDOW_PITCH)
	{
		strValue = StringUtil::ToString(m_fRotateX).c_str();
		return true;
	}
	else if (strName == GWP_ENTITY_WINDOW_YAW)
	{
		strValue = StringUtil::ToString(m_fRotateY).c_str();
		return true;
	}
	else if (strName == GWP_ENTITY_WINDOW_ROLL)
	{
		strValue = StringUtil::ToString(m_fRotateZ).c_str();
		return true;
	}
	else if (strName == GWP_ENTITY_WINDOW_SCALE)
	{
		strValue = StringUtil::ToString(m_fScaleClientSet).c_str();
		return true;
	}

	return false;
}

void GuiEntityWindow::GetPropertyList(GUIPropertyList& properlist)
{
	BaseClass::GetPropertyList(properlist);
	
	properlist.push_back(GWP_ENTITY_WINDOW_PITCH);
	properlist.push_back(GWP_ENTITY_WINDOW_YAW);
	properlist.push_back(GWP_ENTITY_WINDOW_ROLL);
	properlist.push_back(GWP_ENTITY_WINDOW_SCALE);
}

bool GuiEntityWindow::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
{
	if (BaseClass::GetPropertyType(strName, type))
	{
		return true;
	}

	if (strName == GWP_ENTITY_WINDOW_PITCH ||
		strName == GWP_ENTITY_WINDOW_YAW ||
		strName == GWP_ENTITY_WINDOW_ROLL ||
		strName == GWP_ENTITY_WINDOW_SCALE)
	{
		type = GPT_FLOAT;
		return true;
	}

	return false;
}

bool GuiEntityWindow::TouchDown(const Vector2& position)
{
	m_lastXPosition = position.x;

	return 	GUIWindow::TouchDown(position);
}

bool GuiEntityWindow::TouchMove(const Vector2& position)
{
	float delta = position.x - m_lastXPosition;
	m_lastXPosition = position.x;

	float old = GetRotateY();
	SetRotateY(old + delta);

	return 	GUIWindow::TouchMove(position);
}

void GuiEntityWindow::Destroy()
{
	BaseClass::Destroy();
}


}
