#include "CameraGod.h"

namespace BLOCKMAN
{

CameraGod::CameraGod()
	: m_moveMode(MOVEMODE_NONE)
	, m_isLButtonDown(false)
	, m_isDragCamera(false)
	, m_isRButtonDown(false)
	, m_moveSpeed(0.0f)
	, m_maxMoveSpeed(4.0f)
	, m_cameraYaw(0)
	, m_cameraPitch(0)
	, m_mainCamera(NULL)
	, m_pCameraParent(NULL)
	, m_pCameraNode(NULL)
	, m_shiftGear(1)
{
#ifdef LORD_PLATFORM_WINDOWS
	KeyEventManager::Instance()->addListener(this);
#endif
	memset(m_keys, 0, 128);
}

CameraGod::~CameraGod()
{
	m_pCameraParent->destroyChild(m_pCameraNode);
	SceneManager::Instance()->getRootNode()->destroyChild(m_pCameraParent);
#ifdef LORD_PLATFORM_WINDOWS
	KeyEventManager::Instance()->delListener(this);
#endif
}

void CameraGod::init()
{
	m_mainCamera = SceneManager::Instance()->getMainCamera();
	m_mainCamera->setPosition(Vector3(0.f, 70.f, 0.f));
	m_pCameraParent = SceneManager::Instance()->getRootNode()->createChild();
	m_pCameraNode = m_pCameraParent->createChild();
	m_pCameraNode->setWorldPosition(m_mainCamera->getPosition());
	m_pCameraNode->update();
}

int CameraGod::onKeyDown(int keyCode)
{
	m_keys[keyCode] = true;
	return true;
}

int CameraGod::onKeyUp(int keyCode)
{
	m_keys[keyCode] = false;
	if (keyCode == 'r' || keyCode == 'R')
	{
		m_shiftGear = ((++m_shiftGear) & 0xF) + 1;
	}
	return true;
}

int CameraGod::OnButtonDown(MOUSE_BUTTON btnType, int x, int y)
{
	if (btnType == MOUSE_BUTTON_LEFT)
	{
		BeginDoCamera();
		m_ButtonPos.x = x;
		m_ButtonPos.y = y;
	}
	else if (btnType == MOUSE_BUTTON_RIGHT)
	{
		m_isRButtonDown = true;
		m_ButtonPos.x = x;
		m_ButtonPos.y = y;
	}
	return true;
}

int CameraGod::onButtonUp(MOUSE_BUTTON btnType, int x, int y)
{
	if (btnType == MOUSE_BUTTON_LEFT)
	{
		EndDoCamera();
	}
	else if (btnType == MOUSE_BUTTON_RIGHT)
	{
		m_isRButtonDown = false;
		m_isDragCamera = false;
	}
	return true;
}

int CameraGod::onMouseMove(int x, int y)
{
	m_DeltaPos.x = x - m_ButtonPos.x;
	m_DeltaPos.y = y - m_ButtonPos.y;

	if (m_isLButtonDown)
	{
		m_isDragCamera = true;
		DoMouseCamera();
		m_ButtonPos = Vector2i(x, y);
	}
	else if (m_isRButtonDown)
	{
		m_isDragCamera = true;
		LORD::Quaternion orientation = m_mainCamera->getOritation();
		m_ButtonPos = Vector2i(x, y);
		LORD::Vector3 vec(-m_DeltaPos.x*0.01f, m_DeltaPos.y*0.01f, 0.0f);
		LORD::Vector3 trans = orientation * vec;
		LORD::Vector3 position = m_mainCamera->getPosition();
		position += trans;
		m_mainCamera->setPosition(position);
	}

	return true;
}

void CameraGod::update(float dt)
{
	DoKeyEvent();
	DoKeyCamera(dt);
}

void CameraGod::BeginDoCamera()
{
	m_moveSpeed = 0.0f;
	m_isLButtonDown = true;
	m_isDragCamera = false;
}

void CameraGod::EndDoCamera()
{
	m_isLButtonDown = true;
	m_isDragCamera = false;
}


void CameraGod::DoKeyCamera(float dt)
{
	if (MOVEMODE_NONE == m_moveMode)
	{
		m_moveSpeed = 0.0f;
	}
	else
	{
		LORD::Vector3 position = m_mainCamera->getPosition();

		if (MOVEMODE_FORWARD&m_moveMode)
			position = m_mainCamera->getPosition() + m_mainCamera->getDirection()*m_moveSpeed * dt;

		if (MOVEMODE_BACKWARD&m_moveMode)
			position = m_mainCamera->getPosition() - m_mainCamera->getDirection()*m_moveSpeed * dt;

		if (MOVEMODE_LEFTWARD&m_moveMode)
			position = m_mainCamera->getPosition() - m_mainCamera->getRight()*m_moveSpeed * dt;

		if (MOVEMODE_RIGHTWARD&m_moveMode)
			position = m_mainCamera->getPosition() + m_mainCamera->getRight()*m_moveSpeed * dt;

		if (MOVEMODE_UP & m_moveMode)
			position = m_mainCamera->getPosition() + m_mainCamera->getUp()*m_moveSpeed * dt;

		if (MOVEMODE_DOWN & m_moveMode)
			position = m_mainCamera->getPosition() - m_mainCamera->getUp()*m_moveSpeed * dt;

		m_mainCamera->setPosition(position);
		m_pCameraNode->setWorldPosition(position);
		m_pCameraNode->update(true);
		m_moveSpeed = m_maxMoveSpeed*m_shiftGear;
	}
}


void CameraGod::DoMouseCamera()
{
	m_cameraYaw = float(m_DeltaPos.x) / (300.f);
	m_cameraPitch = float(m_DeltaPos.y) / (300.f);

	m_mainCamera->yaw(-m_cameraYaw);
	m_mainCamera->pitch(-m_cameraPitch);

	m_mainCamera->update();
}


void CameraGod::DoKeyEvent()
{
	///KeyDown 
	if (m_keys[0x57])
		m_moveMode |= MOVEMODE_FORWARD;
	if (m_keys[0x53])
		m_moveMode |= MOVEMODE_BACKWARD;
	if (m_keys[0x41])
		m_moveMode |= MOVEMODE_LEFTWARD;
	if (m_keys[0x44])
		m_moveMode |= MOVEMODE_RIGHTWARD;
	if (m_keys[0x51])
		m_moveMode |= MOVEMODE_UP;
	if (m_keys[0x45])
		m_moveMode |= MOVEMODE_DOWN;

	if (!m_keys[0x57])
		m_moveMode &= ~MOVEMODE_FORWARD;
	if (!m_keys[0x53])
		m_moveMode &= ~MOVEMODE_BACKWARD;
	if (!m_keys[0x41])
		m_moveMode &= ~MOVEMODE_LEFTWARD;
	if (!m_keys[0x44])
		m_moveMode &= ~MOVEMODE_RIGHTWARD;
	if (!m_keys[0x51])
		m_moveMode &= ~MOVEMODE_UP;
	if (!m_keys[0x45])
		m_moveMode &= ~MOVEMODE_DOWN;
}
}
