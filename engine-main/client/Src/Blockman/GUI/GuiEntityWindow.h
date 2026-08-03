/********************************************************************
filename: 	gui_entityWindow.h
file path:	dev\client\Src\Blockman\GUI

version:	1
author:		ajohn
company:	supernano
date:		2017-09-22
*********************************************************************/

#ifndef __GUI_ENTITY_WINDOW__
#define __GUI_ENTITY_WINDOW__

#include "GUI/CustomGuiWindow.h"

#include "UI/GUIWindow.h"

using namespace LORD;

namespace BLOCKMAN
{

class EntityPlayer;

class GuiEntityWindow : public GUIWindow
{
private:
	using BaseClass = GUIWindow;

protected:
	void UpdateEntityArea();

public:
	GuiEntityWindow(const GUIWindowType& nType, const GUIString& strName);
	~GuiEntityWindow() = default;

	void		setPlayer(EntityPlayer* player) { m_player = player; }
	EntityPlayer* getPlayer() { return m_player; }

	void		SetRotateX(float fRotate) { m_fRotateX = fRotate; }
	void		SetRotateY(float fRotate) { m_fRotateY = fRotate; }
	void		SetRotateZ(float fRotate) { m_fRotateZ = fRotate; }
	float		GetRotateX() { return m_fRotateX; }
	float		GetRotateY() { return m_fRotateY; }
	float		GetRotateZ() { return m_fRotateZ; }
	void		SetActorScale(float fScale) { m_fScaleClientSet = fScale; }
	void		SetParmaScale(float fScale) { m_fParmaScale = fScale; }

	/** implement override functions from CustomGuiWindow, GUIWindow. */
	virtual void UpdateSelf(ui32 nTimeElapse) override;
	virtual void DrawSelf() override;
	virtual void Destroy() override;
	virtual void OnBeforeDraw() override;
	virtual void OnEndDraw() override;
	virtual bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
	virtual bool GetProperty(const GUIString& strName, GUIString& strValue) override;
	virtual void GetPropertyList(GUIPropertyList& properlist) override;
	virtual bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;
	virtual bool TouchDown(const Vector2& position) override;
	virtual bool TouchMove(const Vector2& position) override;

protected:
	EntityPlayer*	m_player = nullptr;

	float			m_fScaleFactor = 0.0f;
	float			m_fRotateX = 0.0f;
	float			m_fRotateY = 0.0f;
	float			m_fRotateZ = 0.0f;
	float			m_fScaleClientSet = 0.0f;

	float			m_fParmaScale = 0.0f;
	GUIString		m_strActorName;

	Matrix4			m_worldMat;

	float			m_lastXPosition = 0.0f;
};

}

#endif // !__GUI_ITEM_STACK_HEADER__
