#include "Touch.h"
#include "Object/Root.h"
#include "GUIWindowManager.h"
#include "GUIWindow.h"
#include "GUISystem.h"

namespace LORD
{
	Touch::Touch(TOUCH_TYPE type, LORD::ui32 time)
		: m_type(type)
		, m_timeStamp(time)
		, m_startPointCaptured(false)
		, m_startPoint(Vector2::ZERO)
		, m_point(Vector2::ZERO)
		, m_prevPoint(Vector2::ZERO)
		, m_longTouchFlag(false)
		, m_scrollCancelFlag(false)
		, m_index(-1)
		, m_moved(false)
	{

	}

	Touch::~Touch()
	{

	}

	//---------------------------------------------------------------------
	TouchManager* TouchManager::Instance()
	{
		return ms_pSingleton;
	}

	TouchManager::TouchManager()
		: m_indexBitsUsed(0)
	{
		m_touchIdMap.clear();
		memset(m_touches, 0, sizeof(Touch*)* MAX_TOUCHES);
	}

	TouchManager::~TouchManager()
	{
	}

	int TouchManager::_getUnUsedIndex()
	{
		int i;
		int temp = m_indexBitsUsed;

		for (i = 0; i < MAX_TOUCHES; i++) {
			if (!(temp & 0x00000001)) {
				m_indexBitsUsed |= (1 << i);
				return i;
			}

			temp >>= 1;
		}

		// all bits are used
		return -1;
	}

	void TouchManager::_removeUsedIndexBit(int index)
	{
		if (index < 0 || index >= MAX_TOUCHES)
		{
			return;
		}

		unsigned int temp = 1 << index;
		temp = ~temp;
		m_indexBitsUsed &= temp;
	}

	Touch* TouchManager::createTouch(intptr_t id, TOUCH_TYPE type, LORD::ui32 timeStamp)
	{
		int unusedIndex = _getUnUsedIndex();

		// The touches is more than MAX_TOUCHES ?
		if (unusedIndex == -1) {
			LordLogError("The touches is more than MAX_TOUCHES, unusedIndex = %d", unusedIndex);
			return NULL;
		}

		Touch* touch = m_touches[unusedIndex] = LordNew Touch(type, timeStamp);

		touch->setIndex(unusedIndex);

		m_touchIdMap.insert(std::make_pair(id, unusedIndex));

		return touch;
	}

	void TouchManager::destroyTouch(intptr_t id)
	{
		LORD::map<intptr_t, int>::type::iterator iter = m_touchIdMap.find(id);
		if (iter != m_touchIdMap.end())
		{
			LordSafeDelete(m_touches[iter->second]);
			_removeUsedIndexBit(iter->second);

			m_touchIdMap.erase(id);
		}
	}

	Touch* TouchManager::getTouch(intptr_t id)
	{
		LORD::map<intptr_t, int>::type::iterator iter = m_touchIdMap.find(id);
		if (iter != m_touchIdMap.end())
		{
			return m_touches[iter->second];
		} 
		else
		{
			return NULL;
		}
	}

	void TouchManager::tick(ui32 dt)
	{
		ui32 time = Root::Instance()->getCurrentTime();

		LORD::map<intptr_t, int>::type::iterator iter = m_touchIdMap.begin();
		for (; iter != m_touchIdMap.end(); ++iter)
		{
			if (iter->second < 0 || iter->second >= MAX_TOUCHES)
				continue;
			Touch* touch = m_touches[iter->second];
			if (touch)
			{
				if (time - touch->getTimeStamp() > 300)
				{
					if (touch->getType() == TOUCH_TYPE_UI)
					{
						GUIWindow* window = GUIWindowManager::Instance()->GetGUIWindow(touch->getWindowName());

						GUIWindow* nowWindow = GUISystem::Instance()->GetTargetGUIWindow(touch->getTouchPoint());

						if (touch->getLongTouchFlag() == false && window == nowWindow && window && window->isEnableLongTouch())
						{
							window->onLongTouchStart(touch);

							touch->setLongTouchFlag(true);
						}
					}
					else {
						touch->setLongTouchFlag(true);
					}
				}
			}
		}
	}

	int TouchManager::getSceneTouchCount()
	{
		int count = 0;
		LORD::map<intptr_t, int>::type::iterator iter = m_touchIdMap.begin();
		for (; iter != m_touchIdMap.end(); ++iter)
		{
			Touch* touch = m_touches[iter->second];
			if (touch && touch->getType() == TOUCH_TYPE_SCENE)
			{
				++count;
			}
		}

		return count;
	}

	Touch* TouchManager::getSceneTouch1()
	{
		LORD::map<intptr_t, int>::type::iterator iter = m_touchIdMap.begin();
		for (; iter != m_touchIdMap.end(); ++iter)
		{
			Touch* touch = m_touches[iter->second];
			if (touch && touch->getType() == TOUCH_TYPE_SCENE)
			{
				return touch;
			}
		}

		return NULL;
	}

	Touch* TouchManager::getSceneTouch2()
	{
		int count = 0;

		LORD::map<intptr_t, int>::type::iterator iter = m_touchIdMap.begin();
		for (; iter != m_touchIdMap.end(); ++iter)
		{
			Touch* touch = m_touches[iter->second];
			if (touch && touch->getType() == TOUCH_TYPE_SCENE)
			{
				++count;
			}

			if (count == 2)
			{
				return touch;
			}
		}

		return NULL;
	}

	void KeyEventManager::addListener(KeyEventListener* listener)
	{
		if(std::find(m_listeners.begin(), m_listeners.end(), listener) == m_listeners.end()) 
			m_listeners.push_back(listener);
	}

	void KeyEventManager::delListener(KeyEventListener* listener)
	{
		KeyListenArr::iterator it = std::find(m_listeners.begin(), m_listeners.end(), listener);
		if (it != m_listeners.end())
			m_listeners.erase(it);
	}

	int KeyEventManager::onKeyUp(int keyCode)
	{
		int nResult = false;
		for (KeyListenArr::iterator it = m_listeners.begin(); it!=m_listeners.end(); ++it)
		{
			nResult = (*it)->onKeyUp(keyCode);
			if (nResult)
				break;
		}
		return nResult;
	}

	int KeyEventManager::onKeyDown(int keyCode)
	{
		int nResult = false;
		for (KeyListenArr::iterator it = m_listeners.begin(); it!=m_listeners.end(); ++it)
		{
			nResult = (*it)->onKeyDown(keyCode);
			if (nResult)
				break;
		}
		return nResult;
	}

	int KeyEventManager::onButtonDown(MOUSE_BUTTON btnType, int x, int y)
	{
		int nResult = false;
		for (KeyListenArr::iterator it = m_listeners.begin(); it!=m_listeners.end(); ++it)
		{
			nResult = (*it)->OnButtonDown(btnType, x, y);
			if (nResult)
				break;
		}
		return nResult;
	}

	int KeyEventManager::onButtonUp(MOUSE_BUTTON btnType, int x, int y)
	{
		int nResult = false;
		for (KeyListenArr::iterator it = m_listeners.begin(); it!=m_listeners.end(); ++it)
		{
			nResult = (*it)->onButtonUp(btnType, x, y);
			if (nResult)
				break;
		}
		return nResult;
	}

	int KeyEventManager::onMouseMove(int x, int y)
	{
		int nResult = false;
		for (KeyListenArr::iterator it = m_listeners.begin(); it!=m_listeners.end(); ++it)
		{
			nResult = (*it)->onMouseMove(x, y);
			if (nResult)
				break;
		}
		return nResult;
	}
}

