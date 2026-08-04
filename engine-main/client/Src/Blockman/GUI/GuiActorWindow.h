/********************************************************************
filename: 	GuiActorWindow.h
file path:	dev\client\Src\Blockman\GUI

version:	1
author:		ajohn
company:	supernano
date:		2017-11-17
*********************************************************************/

#ifndef __GUI_ACTOR_WINDOW__
#define __GUI_ACTOR_WINDOW__

#include "GUI/CustomGuiWindow.h"

#include "UI/GUIWindow.h"

using namespace LORD;

namespace BLOCKMAN
{

class EntityPlayer;

class GuiActorWindow : public GUIWindow
{
private:
	using BaseClass = GUIWindow;

public:
	GuiActorWindow(GUIWindowType nType, const GUIString& strName);
	virtual ~GuiActorWindow();

	virtual void	UpdateSelf(ui32 nTimeElapse);

	virtual void	DrawSelf();

	void			SetActor(ActorObject* actor);
	void			SetActor(const GUIString& strActorName, const GUIString& strSkillName = "idle", float Rotate = 0.0f);
	int				SetSkillName(const GUIString& strSkillName);

	ActorObject*	GetActor() { return m_pActor; }

	virtual void	OnBeforeDraw();
	virtual void	OnEndDraw();

	void			SetRotateX(float fRotate) { m_fRotateX = fRotate; }
	void			SetRotateY(float fRotate) { m_fRotateY = fRotate; }
	void			SetRotateZ(float fRotate) { m_fRotateZ = fRotate; }

	float			GetRotateX() { return m_fRotateX; }
	float			GetRotateY() { return m_fRotateY; }
	float			GetRotateZ() { return m_fRotateZ; }

	void			SetActorScale(float fScale) { m_fScaleClientSet = fScale; }

	void			SetParmaScale(float fScale) { m_fParmaScale = fScale; }

	virtual bool	SetProperty(const GUIString& strName, const GUIString& strValue);
	virtual bool	GetProperty(const GUIString& strName, GUIString& strValue);
	virtual void	GetPropertyList(GUIPropertyList& properlist);
	virtual bool	GetPropertyType(const GUIString& strName, GUIPropertyType& type);

	virtual bool	TouchDown(const Vector2& position);

	virtual bool	TouchMove(const Vector2& position);

	void				reflreshArmor(EntityPlayer* pPlayer);

	void				UseBodyPart(const String& masterName, const String &slaveName);
	void				UnloadBodyPart(const String& masterName);
	void				SetCustomColor(const Color& color);
	void				PlaySkill(const String& skillName);
	void				SetHoldItem(EntityPlayer* pPlayer, int itemId);

protected:
	void			UpdateActorArea();

protected:

	ActorObject*	m_pActor = nullptr;

	float			m_fScaleFactor = 0.0f;
	float			m_fRotateX = 0.0f;
	float			m_fRotateY = 0.0f;
	float			m_fRotateZ = 0.0f;
	float			m_fScaleClientSet = 0.0f;

	float			m_fParmaScale = 0.0f;	//外部缩放参数
	GUIString		m_strActorName;
	GUIString		m_strSkillName;

	float			m_lastXPosition = 0.0f;

};

}

#endif // !__GUI_ITEM_STACK_HEADER__