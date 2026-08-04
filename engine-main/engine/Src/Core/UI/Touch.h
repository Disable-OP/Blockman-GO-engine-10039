/********************************************************************
filename: 	Touch.h
file path:	dev\engine\Src\Core\UI\

version:	1
author:		ajohn from CEGUI
company:	supernano
*********************************************************************/

#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "Core.h"
#include "GUIString.h"

namespace LORD
{
	enum TOUCH_TYPE
	{
		TOUCH_TYPE_INVALID = -1,
		TOUCH_TYPE_UI,
		TOUCH_TYPE_SCENE,
	};

	class Touch : public LORD::ObjectAlloc
	{
	public:
		Touch(TOUCH_TYPE type, LORD::ui32 time);
		~Touch();

		void setWindowName(const LORD::GUIString& name) { m_windowName = name; }
		const LORD::GUIString& getWindowName() const { return m_windowName; }

		void setTouchPoint(const LORD::Vector2& point, float tolerance = 10)
		{
			m_prevPoint = m_point;
			m_point.x = point.x;
			m_point.y = point.y;
			if (!m_startPointCaptured)
			{
				m_startPoint = m_point;
                m_prevPoint = m_point;
				m_startPointCaptured = true;
			}

			if (!m_moved && (Math::Abs(m_point.x - m_startPoint.x) > tolerance || Math::Abs(m_point.y - m_startPoint.y) > tolerance)) {
				m_moved = true;
			}
		}

		const Vector2&	getTouchPoint() const { return m_point; }
		const Vector2&	getStartPoint() const { return m_startPoint; }
		const Vector2&	getPrevPoint() const { return m_prevPoint; }

		void	setIndex(int index) { m_index = index; }
		int		getIndex() const { return m_index; }

		TOUCH_TYPE getType() const { return m_type; }

		void	setTouchType(TOUCH_TYPE type) { this->m_type = type; }
		void	setLongTouchFlag(bool flag) { m_longTouchFlag = flag; }
		bool	getLongTouchFlag() const { return m_longTouchFlag; }

		void	setScrollCancelFlag(bool flag) { m_scrollCancelFlag = flag; }
		bool	getScrollCancelFlag() const { return m_scrollCancelFlag; }

		bool	isMoved() const {
			return m_moved;
		}

		bool	isClick() const {
			if (!m_moved && !m_longTouchFlag) {
				return true;
			}
			else {
				return false;
			}
		}

		bool	isLongTouchBegin() const {
			if (!m_moved && m_longTouchFlag) {
				return true;
			}
			else {
				return false;
			}
		}

		ui32	getTimeStamp() const { return m_timeStamp; }

	private:
		// 是场景还是ui
		TOUCH_TYPE			m_type;
		// 如果是ui的话，按下的那个window的name
		LORD::GUIString		m_windowName;
		// 按下时的时间戳
		LORD::ui32			m_timeStamp;

		bool				m_startPointCaptured;
		LORD::Vector2		m_startPoint;
		LORD::Vector2		m_point;
		LORD::Vector2		m_prevPoint;

		bool				m_longTouchFlag;
		bool				m_scrollCancelFlag;

		int					m_index;
		bool				m_moved;
	};

	class LORD_CORE_API TouchManager : public ObjectAlloc, public Singleton<TouchManager>
	{
	public:

		enum TOUCH_COUNT
		{
			MAX_TOUCHES = 5,
		};

		TouchManager();
		~TouchManager();

		static TouchManager* Instance();

		Touch*	createTouch(intptr_t id, TOUCH_TYPE type, LORD::ui32 timeStamp);
		void	destroyTouch(intptr_t id);

		Touch*  getTouch(intptr_t id);

		void	tick(ui32 dt);

		int		getSceneTouchCount();

		Touch*	getSceneTouch1();

		Touch*	getSceneTouch2();

	protected:
		int		_getUnUsedIndex();

		void	_removeUsedIndexBit(int index);

	private:

		LORD::map<intptr_t, int>::type	m_touchIdMap;
		unsigned int					m_indexBitsUsed;
		Touch*							m_touches[MAX_TOUCHES];
	};

	enum MOUSE_BUTTON
	{
		MOUSE_BUTTON_INVALID = -1,

		MOUSE_BUTTON_LEFT,
		MOUSE_BUTTON_RIGHT,
		MOUSE_BUTTON_MIDDLE,
	};

	class LORD_CORE_API KeyEventListener
	{
	public:
		virtual int onKeyDown(int keyCode) = 0;
		virtual int onKeyUp(int keyCode) = 0;
		virtual int OnButtonDown(MOUSE_BUTTON btnType, int x, int y) = 0;
		virtual int onButtonUp(MOUSE_BUTTON btnType, int x, int y) = 0;
		virtual int onMouseMove(int x, int y) = 0;
	};

	typedef LORD::list<KeyEventListener*>::type KeyListenArr;

	class LORD_CORE_API KeyEventManager : public ObjectAlloc, public Singleton<KeyEventManager>
	{
	public:
		void addListener(KeyEventListener* listener);
		void delListener(KeyEventListener* listener);

		int onKeyDown(int keyCode);
		int onKeyUp(int keyCode);
		int onButtonDown(MOUSE_BUTTON btnType, int x, int y);
		int onButtonUp(MOUSE_BUTTON btnType, int x, int y);
		int onMouseMove(int x, int y);
	protected:
		
		KeyListenArr m_listeners;
	};
}


#endif