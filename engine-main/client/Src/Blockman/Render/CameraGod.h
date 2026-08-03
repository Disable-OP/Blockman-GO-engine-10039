/********************************************************************
filename: 	CameraGod.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-12-6
*********************************************************************/
#ifndef __CAMERA_GOD_HEADER__
#define __CAMERA_GOD_HEADER__

#include "Core.h"
using namespace LORD;

#include "UI/Touch.h"

namespace BLOCKMAN
{

class CameraGod : public KeyEventListener, public ObjectAlloc
{
public:
	enum MOVEMODE {
		MOVEMODE_NONE = 0,
		MOVEMODE_FORWARD = 1 << 0,
		MOVEMODE_BACKWARD = 1 << 1,
		MOVEMODE_LEFTWARD = 1 << 2,
		MOVEMODE_RIGHTWARD = 1 << 3,
		MOVEMODE_UP = 1 << 4,
		MOVEMODE_DOWN = 1 << 5,
	};

public:
	CameraGod();
	~CameraGod();

	void init();

	int onKeyUp(int keyCode);
	int onKeyDown(int keyCode);
	int OnButtonDown(MOUSE_BUTTON btnType, int x, int y);
	int onButtonUp(MOUSE_BUTTON btnType, int x, int y);
	int onMouseMove(int x, int y);

	int onEventHold(int keyCode) { return onKeyDown(keyCode); }
	int onEventRelease(int keyCode) { return onKeyUp(keyCode); }
	int onTouchBegin(int x, int y) { return OnButtonDown(MOUSE_BUTTON_LEFT, x, y); }
	int onTouchMove(int x, int y) { return onMouseMove(x, y); }
	int onTouchEnd(int x, int y) { return onButtonUp(MOUSE_BUTTON_LEFT, x, y); }

	void update(float dt);

protected:
	void BeginDoCamera(void);                                                
	void EndDoCamera(void);                                                   

	void DoKeyEvent();
	void DoMouseCamera();														
	void DoKeyCamera(float dt);

protected:
	ui32 m_moveMode;                                                         
	bool m_isLButtonDown;
	bool m_isDragCamera;
	bool m_isRButtonDown;
	float m_moveSpeed;                                                        
	float m_maxMoveSpeed;
	int	  m_shiftGear;

	float m_cameraYaw;
	float m_cameraPitch;

	Vector2i m_ButtonPos;
	Vector2i m_DeltaPos;
	ui8	 m_keys[128];

	Camera* m_mainCamera;
	SceneNode *m_pCameraParent;										  
	SceneNode *m_pCameraNode;							
};

};

#endif
